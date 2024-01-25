#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <inttypes.h>
#include <limits.h>
#include <errno.h>

#include <string.h>
#include <fcntl.h>      /* open */
#include <unistd.h>     /* exit */
#include <sys/ioctl.h>  /* ioctl */
#include <sys/mman.h>
#include <numa.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>

#include <ptdump.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#if (defined(GREEN_MARL) || defined(GUPS))
#define DISABLE_SIGNAL_IN_MAIN
#endif

/* configuration options */

// use mfd instead of shm for creating the memory file
//#define CONFIG_USE_MFD 1

#define CONFIG_HAVE_MODIFIED_KERNEL 1

///< the size of the buffer in bits
#define CONFIG_PT_BUFFER_SIZE_BITS 31

///< the name of the shared memory file created
#define CONFIG_SHM_FILE_NAME "/tmp/alloctest-bench"

///< virtual base address of the table to be mapped. Currently PML4[128 + x]
#define CONFIG_TABLE_MAP_BASE(x) (uintptr_t)((128UL + (x)) * (512UL << 30))

#define CONFIG_STATUS_PRINT stderr

#define CONFIG_MAX_ADDRESS_BITS 48

#define CONFIG_MMAP_FLAGS MAP_SHARED | MAP_FIXED | MAP_NORESERVE

#define CONFIG_MMAP_PROT PROT_READ | PROT_WRITE

#define CHECK_NODE(_n, _s)                                                      \
    do {                                                                        \
        if (_n < 0 || _n >= numa_num_configured_nodes()) {                      \
            fprintf(stderr, "WARNING: provided numa node for " _s " was "       \
                            " %d out of supported range! setting to 0.\n", _n); \
            _n = 0;                                                             \
        }                                                                       \
    } while(0);

#ifndef MAP_HUGE_2MB
#define MAP_HUGE_2MB    (21 << MAP_HUGE_SHIFT)
#endif


FILE *opt_file_out = NULL;     ///< standard output
static bool opt_interference  = false;

#ifdef CONFIG_USE_MFD
#include <linux/memfd.h>
#include <asm/unistd_64.h>

static inline int memfd_create(const char *name, unsigned int flags) {
    return syscall(__NR_memfd_create, name, flags);
}
#endif

#ifdef CONFIG_USE_PAPI

#include <papi.h>

///< the maximum number of counters to be used
#define MAX_HW_COUNTERS 3

///< the number of available hardware counters
static int papi_num_hw_counters = -1;

///< which events to record
static int hw_events[] = {
    PAPI_TOT_CYC,
    PAPI_TLB_DM,
    PAPI_L3_TCM,
    PAPI_RES_STL
};

///< strings representing the events
static char *hw_event_str[] = {
    "totalcycles",
    "dtlb-miss",
    "total-l3-miss",
    "cycles-stalled"
};

static int initialize_papi(void) {

    /* Initialie the PAPI Library */
    if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT) {
        fprintf(stderr, "PAPI library init error!\n");
        return -1;
    }

    if ((papi_num_hw_counters = PAPI_num_counters()) <= PAPI_OK)  {
        fprintf(stderr, "No hardware counters available\n");
    }

    if (papi_num_hw_counters >= MAX_HW_COUNTERS) {
        papi_num_hw_counters = MAX_HW_COUNTERS;
    }

    return 0;
}


static void start_papi(void)
{
    if (PAPI_start_counters(hw_events, papi_num_hw_counters) != PAPI_OK) {
        fprintf(stderr, "failed to start the counters\n");
    }
}

static void stop_papi(void)
{
    long long int values[papi_num_hw_counters];

    /* Read the counters */
    if (PAPI_stop_counters(values, papi_num_hw_counters) != PAPI_OK) {
        fprintf(stderr, "failed to start the counters\n");
        return;
    }

    for (size_t i = 0; i < papi_num_hw_counters; i++) {
        fprintf(opt_file_out, "<%s>%lld</%s>\n", hw_event_str[i], values[i],
                hw_event_str[i]);
    }
}

#else
static inline int initialize_papi(void) { return 0;}
static inline int start_papi(void) { }
static inline int stop_papi(void) { }
#endif


/*
 * ============================================================================
 * CRC32 Checksum
 * ============================================================================
 */

static uint32_t crc32_for_byte(uint32_t r)
{
    for(int j = 0; j < 8; ++j) {
        r = (r & 1? 0: (uint32_t)0xEDB88320L) ^ r >> 1;
    }
    return r ^ (uint32_t)0xFF000000L;
}

static void crc32(const void *data, size_t n_bytes, uint32_t* crc)
{
    static uint32_t table[0x100];
    if(!*table) {
        for(size_t i = 0; i < 0x100; ++i) {
            table[i] = crc32_for_byte(i);
        }
    }

    for(size_t i = 0; i < n_bytes; ++i) {
        *crc = table[(uint8_t)*crc ^ ((uint8_t*)data)[i]] ^ *crc >> 8;
    }
}


/*
 * ============================================================================
 * PT Dump
 * ============================================================================
 */

static bool opt_dump_pt_crc  = false;
static bool opt_dump_pt_full = false;
static void *opt_dump_buf = NULL;

///< marshall the arguments for the buffer passed to the kernel module
#define PTDUMP_MARSHALL_ARGS(ptr, sz) ((uint64_t) sz << 48 | (uint64_t)ptr)
#define PTDUMP_MARSHALL_ARGS2(ptr, st, en) ((uint64_t) en << 56 \
                                            | (uint64_t) st << 48 \
                                            | (uint64_t)ptr)

#define PTABLE_BASE_MASK(x) ((x) & 0xfffffffff000UL)

static void dump_page_tables(int f, pid_t pid)
{

    fprintf(opt_file_out, "<pagetables>\n");

    void *p = malloc(4);
    long res;

    if (opt_dump_pt_full) {
        struct ptdump *result = (struct ptdump *)opt_dump_buf;
        memset(result, 0, sizeof(*result));
        do {
            res = ioctl(f, PTDUMP_IOCTL_MKCMD(PTDUMP_IOCTL_CMD_DUMP, 0, 256),
                           PTDUMP_IOCTL_MKARGBUF(result, 0));
        } while(res != 0);

        printf("Obtained %lu tables\n", result->num_tables);
        fprintf(opt_file_out, "<ptables>");
        for (unsigned long i = 0; i < result->num_tables; i++) {
            fprintf(opt_file_out, "%09lx ", PTABLE_BASE_MASK(result->table[i].base) >> 12);
        }
        fprintf(opt_file_out, "</ptables>\n");

        fprintf(opt_file_out, "<codeframes4k>");
        for (unsigned long i = 0; i < result->num_tables; i++) {
            if (PTDUMP_TABLE_EXLEVEL(result->table[i].base) != 1) {
                continue;
            }
            for (int j = 0; j < 512; j++) {
                if ((result->table[i].entries[j] & 0x1)) {
                    if (!(result->table[i].entries[j] & (0x1UL << 63))) {
                        fprintf(opt_file_out, "%09lx ", PTABLE_BASE_MASK(result->table[i].entries[j]) >> 12);
                    }
                }
            }
        }
        fprintf(opt_file_out, "</codeframes4k>\n");

        fprintf(opt_file_out, "<dataframes4k>");
        for (unsigned long i = 0; i < result->num_tables; i++) {
            if (PTDUMP_TABLE_EXLEVEL(result->table[i].base) != 1) {
                continue;
            }
            for (int j = 0; j < 512; j++) {
                if ((result->table[i].entries[j] & 0x1)) {
                    if ((result->table[i].entries[j] & (0x1UL << 63))) {
                       fprintf(opt_file_out, "%09lx ", PTABLE_BASE_MASK(result->table[i].entries[j]) >> 12);
                    }
                }
            }
        }
        fprintf(opt_file_out, "</dataframes4k>\n");

        fprintf(opt_file_out, "<codeframes2M>");
        for (unsigned long i = 0; i < result->num_tables; i++) {
            if (PTDUMP_TABLE_EXLEVEL(result->table[i].base) != 2) {
                continue;
            }
            for (int j = 0; j < 512; j++) {
                if ((result->table[i].entries[j] & 0x1) && (result->table[i].entries[j] & (0x1 << 7))) {
                    if (!(result->table[i].entries[j] & (0x1UL << 63))) {
                        fprintf(opt_file_out, "%07lx ", PTABLE_BASE_MASK(result->table[i].entries[j]) >> 21);
                    }
                }
            }
        }
        fprintf(opt_file_out, "</codeframes2M>\n");

        fprintf(opt_file_out, "<dataframes2M>");
        for (unsigned long i = 0; i < result->num_tables; i++) {
            if (PTDUMP_TABLE_EXLEVEL(result->table[i].base) != 2) {
                continue;
            }
            for (int j = 0; j < 512; j++) {
                if ((result->table[i].entries[j] & 0x1) && (result->table[i].entries[j] & (0x1 << 7))) {
                    if ((result->table[i].entries[j] & (0x1UL << 63))) {
                       fprintf(opt_file_out, "%07lx ", PTABLE_BASE_MASK(result->table[i].entries[j]) >> 21);
                    }
                }
            }
        }
        fprintf(opt_file_out, "</dataframes2M>\n");

    } else if (opt_dump_pt_crc) {

        struct ptranges ranges = {0x0};
        struct ptcrc32 crcs  = {0x0};
        res = ioctl(f, PTDUMP_IOCTL_MKCMD(PTDUMP_IOCTL_CMD_RANGES, 0, 256),
                       PTDUMP_IOCTL_MKARGBUF(&ranges, 0));
        if (res == 0) {
            fprintf(opt_file_out, "<ptranges>\n"
            "<l4>0x%012lx..0x%012lx</l4>\n"
            "<table>0x%012lx..0x%012lx</table>\n"
            "<data>0x%012lx..0x%012lx</data>\n"
            "<code>0x%012lx..0x%012lx</code>\n"
            "<kernel>0x%012lx..0x%012lx</kernel>\n"
            "</ptranges>\n",
            ranges.pml4_base, ranges.pml4_limit,
            ranges.pt_base, ranges.pt_limit,
            ranges.data_base, ranges.data_limit,
            ranges.code_base, ranges.code_limit,
            ranges.kernel_base, ranges.kernel_limit);
        } else {
            fprintf(opt_file_out, "<ptranges>INVALID</ptranges>\n");
        }
        res = ioctl(f, PTDUMP_IOCTL_MKCMD(PTDUMP_IOCTL_CMD_CRC32, 0, 256),
                       PTDUMP_IOCTL_MKARGBUF(&crcs, 0));
        if (res == 0) {
            fprintf(opt_file_out, "<crc><l4>0x%08x</l4><l3>0x%08x</l3><l2>0x%08x</l2>"
                                       "<l1>0x%08x</l1><d>0x%08x</d></crc>\n",
                    crcs.pml4, crcs.pdpt, crcs.pdir, crcs.pt, crcs.data);
        } else {
            fprintf(opt_file_out, "<crc>INVALID</crc>\n");
        }

    } else {
        fprintf(opt_file_out, "Page table dumping disabled.\n");
    }

    fprintf(opt_file_out, "</pagetables>\n");
}


int opt_numa_node_pte = 0;
int opt_madvise_advice = MADV_NOHUGEPAGE;
struct bitmask *opt_runmask = NULL;
struct bitmask *opt_allocmask = NULL;
struct bitmask *opt_ptemask = NULL;

extern int real_main(int argc, char *argv[]);

static int ptdump_file;
static pid_t  pid;



void signalhandler( int sig ) {
    fprintf(opt_file_out,  "<sig>Signal %i caught!</sig>\n", sig );

    FILE *fd3 = fopen(CONFIG_SHM_FILE_NAME ".done", "w");

    if (fd3 == NULL) {
        fprintf (stderr, "ERROR: could not create the shared memory file descriptor\n");
        exit(-1);
    }

    usleep(250);


//    dump_page_tables(ptdump_file, pid);

    if (opt_dump_pt_full) {
        free(opt_dump_buf);
    }

    fprintf(opt_file_out, "</benchmark>\n");

    exit(0);
}

#include <sys/time.h>

int main(int argc, char *argv[])
{

    struct timeval tstart, tend;
    gettimeofday(&tstart, NULL);

    for (int i = 0; i < argc; i++) {
        printf("%s ", argv[i]);
    }
    printf("\n");

    if (initialize_papi() == -1) {
        fprintf(stderr, "ERROR: Could not initialize papi.\n");
        return -1;
    }

    int node_data = -1;          ///< the node where data resided
    uint64_t mem  = 1UL << 30;
    opt_file_out = stdout;

    int c;
    while ((c = getopt (argc, argv, "p:d:m:o:fhitc")) != -1) {
        switch (c) {
        case 'p':
            opt_numa_node_pte = strtol(optarg, NULL, 10);
            CHECK_NODE(opt_numa_node_pte, "pagetable node");
            break;
        case 'd':
            node_data = strtol(optarg, NULL, 10);
            CHECK_NODE(node_data, "data node");
            break;
        case 'm':
            mem = strtoull(optarg, NULL, 10) << 30;
            break;
        case 'c':
            opt_dump_pt_crc = true;
            break;
        case 'f':
            opt_dump_pt_full = true;
            break;
        case 't':
            opt_madvise_advice = MADV_HUGEPAGE;
            break;
        case '-' :
            break;
        case 'h':
            printf("usage: %s [-p N] [-d N] [-r N] [-m N] [-o FILE]\n", argv[0]);
            printf("p: pagetable node, d: datanode, r: runnode, m: memory in GB\n");
            return 0;
        case 'o' :
            opt_file_out = fopen(optarg, "a");
            if (opt_file_out == NULL) {
                fprintf (stderr, "Could not open the file '%s' switching to stdout\n", optarg);
                opt_file_out = stdout;
            }
            break;
        case '?':
            switch(optopt) {
                case 'p' :
                case 'r' :
                case 'd' :
                case 'm' :
                case 'o' :
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                    return -1;
                default:
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                return -1;
            }
        }
    }

    struct bitmask *opt_allocmask = NULL;
    if (node_data != -1) {
        opt_allocmask = numa_allocate_nodemask();
        opt_allocmask = numa_bitmask_clearall(opt_allocmask);
        opt_allocmask = numa_bitmask_setbit(opt_allocmask, node_data);
        numa_set_membind(opt_allocmask);
    }

#ifdef _OPENMP
    int num_threads = numa_num_configured_cpus();
    omp_set_num_threads(num_threads);

    fprintf(stderr, "Total CPUs: %d, Total NUMA nodes: %d, Threads used: %d\n",
            numa_num_configured_cpus(),numa_num_configured_nodes(), num_threads);

    omp_set_num_threads(num_threads);

    #pragma omp parallel for
    for (int i = 0; i < num_threads; i++) {
    //    fprintf(stderr, "Threads %d -> core %d\n", omp_get_thread_num(), i);
        cpu_set_t my_set;
        CPU_ZERO(&my_set);
        CPU_SET(i, &my_set);
        sched_setaffinity(0, sizeof(cpu_set_t), &my_set);

        //numa_set_strict(1);
        //numa_set_bind_policy(1);

        if (opt_allocmask != NULL) {
            numa_set_membind(opt_allocmask);
        }
    }

//    omp_set_schedule(omp_sched_static, 1);
#endif

    int prog_argc = 0;
    char **prog_argv = NULL;

    prog_argv = &argv[0];
    prog_argc = argc;

    optind = 1;

    for (int i = 0; i < argc; i ++) {
        if (strcmp("--", argv[i]) == 0) {
            argv[i] = argv[0];
            prog_argv = &argv[i];
            prog_argc = argc - i;
            break;
        }
    }

#ifndef DISABLE_SIGNAL_IN_MAIN
    fprintf (stderr,"signalling readyness to %s\n", CONFIG_SHM_FILE_NAME ".ready");
    FILE *fd2 = fopen(CONFIG_SHM_FILE_NAME ".ready", "w");

    if (fd2 == NULL) {
        fprintf (stderr, "ERROR: could not create the shared memory file descriptor\n");
        exit(-1);
    }

    usleep(250);
#endif

    /* start with output */
    fprintf(opt_file_out, "<benchmark exec=\"%s\">\n", argv[0]);

    fprintf(opt_file_out, "<config>\n");
    #ifdef _OPENMP
    fprintf(opt_file_out, "  <openmp>on</openmp>");
    #else
    fprintf(opt_file_out, "  <openmp>off</openmp>");
    #endif
    fprintf(opt_file_out, "  <memsize>%" PRIu64 "G <memsize>", mem >> 30);
    fprintf(opt_file_out, "  <hugetlb>%d</hugetlb>\n", (opt_madvise_advice == MADV_HUGEPAGE));
    fprintf(opt_file_out, "  <data>%d</data>", node_data);
    fprintf(opt_file_out, "  <interference>%d</interference>\n", opt_interference);
    fprintf(opt_file_out, "</config>\n");

    /* setting the bind policy */
    //numa_set_strict(1);
    //numa_set_bind_policy(1);

    if (opt_dump_pt_full || opt_dump_pt_crc) {
        pid = getpid();
        ptdump_file = open(PTDUMP_FILE, 0);
        if (ptdump_file < 0) {
            fprintf (stderr, "Can't open device file: %s\n", "/proc/ptdumpself");
            return -1;
        }

        if (opt_dump_pt_full) {
            opt_dump_buf = malloc(sizeof(struct ptdump));
        }
    }



  //  dump_page_tables(ptdump_file, pid);

    struct sigaction sigact;
    sigset_t block_set;

    sigfillset( &block_set );
    sigdelset( &block_set, SIGUSR1 );

    sigemptyset( &sigact.sa_mask );
    sigact.sa_flags = 0;
    sigact.sa_handler = signalhandler;
    sigaction( SIGUSR1, &sigact, NULL );

    fprintf(opt_file_out, "<run>\n");
    real_main(prog_argc, prog_argv);
    fprintf(opt_file_out, "</run>\n");

    FILE *fd3 = fopen(CONFIG_SHM_FILE_NAME ".done", "w");

    if (fd3 == NULL) {
        fprintf (stderr, "ERROR: could not create the shared memory file descriptor\n");
        exit(-1);
    }

    usleep(250);

//    dump_page_tables(ptdump_file, pid);

    if (opt_dump_pt_full) {
        free(opt_dump_buf);
    }


    gettimeofday(&tend, NULL);
    printf("Took: %zu.%03zu\n", tend.tv_sec - tstart.tv_sec, 
                            (tend.tv_usec - tstart.tv_usec) / 1000);
    fprintf(opt_file_out, "Took: %zu.%03zu\n", tend.tv_sec - tstart.tv_sec, 
                            (tend.tv_usec - tstart.tv_usec) / 1000);

    fprintf(opt_file_out, "</benchmark>\n");
    fflush(opt_file_out);
    return 0;
}
