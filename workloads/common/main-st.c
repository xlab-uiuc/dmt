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


#ifdef _OPENMP
#include <omp.h>
#endif

/* configuration options */

// use mfd instead of shm for creating the memory file
//#define CONFIG_USE_MFD 1


#if (defined(GREEN_MARL) || defined(GUPS))
#define DISABLE_SIGNAL_IN_MAIN 
#endif 



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


int opt_numa_node_pte = 0;
int opt_madvise_advice = MADV_NOHUGEPAGE;
struct bitmask *opt_runmask = NULL;
struct bitmask *opt_allocmask = NULL;
struct bitmask *opt_ptemask = NULL;

extern int real_main(int argc, char *argv[]);

void signalhandler( int sig ) {
    fprintf(opt_file_out,  "<sig>Signal %i caught!</sig>\n", sig );

    //  FILE *fd3 = fopen(CONFIG_SHM_FILE_NAME ".done", "w");

    // if (fd3 == NULL) {
    //     fprintf (stderr, "ERROR: could not create the shared memory file descriptor\n");
    //     exit(-1);
    // }

    // usleep(250);
        
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

    /* check if NUMA is available, otherwise we don't know how to allocate memory */
    if (numa_available() == -1) {
        fprintf(stderr, "ERROR: Numa not available on this machine.\n");
        return -1;
    }

    /* set the membind */
    struct bitmask *nodes = numa_allocate_nodemask();
    if (nodes == NULL) {
        fprintf(stderr, "ERROR: No memory for nodemask\n");
        return -1;
    }    

    int node_data = 0;          ///< the node where data resided
    int node_run  = 0;          ///< the node where the program is run
    uint64_t mem  = 1UL << 30;
    opt_file_out = stdout;    
    int num_threads = 1;
    int c;
    while ((c = getopt (argc, argv, "p:d:r:m:o:fhistc")) != -1) {       
        switch (c) {
        case 'p':
            opt_numa_node_pte = strtol(optarg, NULL, 10);
            CHECK_NODE(opt_numa_node_pte, "pagetable node");
            break;
        case 'r':
            node_run = strtol(optarg, NULL, 10);
            CHECK_NODE(node_run, "run node");
            break;
        case 'd':
            node_data = strtol(optarg, NULL, 10);
            CHECK_NODE(node_data, "data node");
            break;
        case 'm':
            mem = strtoull(optarg, NULL, 10) << 30;
            break;       
        case 'i' :
            opt_interference = true;
            break;
        case 't':
            opt_madvise_advice = MADV_HUGEPAGE;
            break;      
        case 's':
            num_threads = 0;
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
#if 0
    struct bitmask *tmpnode = numa_allocate_nodemask();
    opt_runmask = numa_bitmask_setbit(numa_bitmask_clearall(tmpnode), node_run);

    tmpnode = numa_allocate_nodemask();
    opt_allocmask = numa_bitmask_setbit(numa_bitmask_clearall(tmpnode), node_data);
    
    tmpnode = numa_allocate_nodemask();
    opt_ptemask = numa_bitmask_setbit(numa_bitmask_clearall(tmpnode), opt_numa_node_pte);


    struct bitmask *from = numa_allocate_nodemask();
    from = numa_bitmask_setall(from);
    from = numa_bitmask_clearbit(from, node_data);  // don't move data pages already on the right node

    fprintf(stderr, "Migrating pages to node %d\n",  node_data);    

    if (numa_migrate_pages(0, from, opt_allocmask)) {
        fprintf(stderr, "FAILED TO MOVE PAGES!!!\n");
    }
 
    for (int i = 0; i < numa_num_configured_cpus(); i++) {
        if (numa_node_of_cpu(i) == node_run) {
            cpu_set_t my_set;
            CPU_ZERO(&my_set);
            CPU_SET(i, &my_set);
            sched_setaffinity(0, sizeof(cpu_set_t), &my_set);
            break;
        }
    }
    numa_set_membind(opt_allocmask);
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

    for (int i = 0; i < prog_argc; i ++) {
        printf("prog_argv[%i]=%s\n", i, prog_argv[i]);
    }

    // fprintf (stderr,"NO signalling readyness to %s\n", CONFIG_SHM_FILE_NAME ".ready");
#ifndef DISABLE_SIGNAL_IN_MAIN 
    // fprintf (stderr,"signalling readyness to %s\n", CONFIG_SHM_FILE_NAME ".ready");
    // FILE *fd2 = fopen(CONFIG_SHM_FILE_NAME ".ready", "w");

    // if (fd2 == NULL) {
    //     fprintf (stderr, "ERROR: could not create the shared memory file descriptor\n");
    //     exit(-1);
    // }

    // usleep(250);
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
    fprintf(opt_file_out, "  <pte>%d</pte>", opt_numa_node_pte);
    fprintf(opt_file_out, "  <run>%d</run>\n", node_run);
    
    fprintf(opt_file_out, "  <interference>%d</interference>\n", opt_interference);
    fprintf(opt_file_out, "</config>\n");

    /* setting the bind policy */
    numa_set_strict(1);
    numa_set_bind_policy(1);

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

    // FILE *fd3 = fopen(CONFIG_SHM_FILE_NAME ".done", "w");

    // if (fd3 == NULL) {
    //     fprintf (stderr, "ERROR: could not create the shared memory file descriptor\n");
    //     exit(-1);
    // }

    usleep(250);

    gettimeofday(&tend, NULL);
    printf("Took: %zu.%03zu\n", tend.tv_sec - tstart.tv_sec, 
                            (tend.tv_usec - tstart.tv_usec) / 1000);
    fprintf(opt_file_out, "Took: %zu.%03zu\n", tend.tv_sec - tstart.tv_sec, 
                            (tend.tv_usec - tstart.tv_usec) / 1000);
        
    fprintf(opt_file_out, "</benchmark>\n");
    return 0;
}
