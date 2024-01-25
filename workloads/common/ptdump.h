/**
 * Copyright (c) 2018, VMware
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the ptdump project. 
 */


/* ioctl interface of the pagetable dump facility */


/*
 *
 */

#define PARAM_GET_PTR(p) (void *)(p & 0xffffffffffff)
#define PARAM_GET_BITS(p) (p >> 48)


// the file node that shows up when we instantiate the kernel module
#define CONFIG_PROCFS_FILE      "ptdump"


///< the procfs file
#define PTDUMP_FILE "/proc/" CONFIG_PROCFS_FILE

/*
 * --------------------------------------------------------------
 * ERRORS
 * --------------------------------------------------------------
 */
#define PTDUMP_ERR_NO_SUCH_PROCESS -1
#define PTDUMP_ERR_NO_SUCH_TASK -2
#define PTDUMP_ERR_ACCESS_RIGHTS    -3
#define PTDUMP_ERR_CMD_INVALID -4


/*
 * --------------------------------------------------------------
 * COMMAND PARAMETERS
 * --------------------------------------------------------------
 */

#define PTDUMP_IOCTL_MKCMD(cmd, ptstart, ptend)   \
    ((unsigned int)(cmd & 0xff)                   \
        | ((unsigned int)(ptstart & 0x1ff) << 8)  \
        | ((unsigned int)(ptend & 0x1ff) << 17)) 

#define PTDUMP_IOCTL_EXCMD(x) ((unsigned int)(x & 0xff))
#define PTDUMP_IOCTL_EXPTSTART(x) ((unsigned int)((x >> 8) & 0x1ff))
#define PTDUMP_IOCTL_EXPTEND(x) ((unsigned int)((x >> 17) & 0x1ff))


#define PTDUMP_IOCTL_MKARGBUF(buf, bits) \
        (((unsigned long)buf) & 0xffffffffffffUL) \
            | ((unsigned long)(bits & 0xff) << 48)
#define PTDUMP_IOCTL_EXBUF(arg) (void *)((unsigned long)arg & 0xffffffffffffUL)
#define PTDUMP_IOCTL_EXBITS(arg) ((unsigned long)(arg >> 48) & 0xff)


/*
 * --------------------------------------------------------------
 * COMMAND: DUMP
 * --------------------------------------------------------------
 */

///< IOCTL command param to dump the page tables
#define PTDUMP_IOCTL_CMD_DUMP     0x3


#define MAX_VASPACE_DUMP (512UL << 30)
#define NUM_L4 1                                    /// 1
#define NUM_L3 (MAX_VASPACE_DUMP / (512UL << 30))    /// 1
#define NUM_L2 (MAX_VASPACE_DUMP / (1UL << 30))      /// 512
#define NUM_L1 (MAX_VASPACE_DUMP / (2UL << 20))      /// 262144 = 512*512

#define PTDUMP_DUMP_SIZE_MAX    (NUM_L1 + NUM_L2 + NUM_L3 + NUM_L4)

#define PTDUMP_TABLE_EXLEVEL(base) (unsigned)(base >> 48)
#define PTDUMP_TABLE_EXBASE(base) (base & 0xffffffffffffUL)
#define PTDUMP_TABLE_MKBASE(base, level) (((unsigned long)level & 0xff) << 48 | (base & 0xffffffffffffUL))

struct ptdump_table 
{
    unsigned long base;
    unsigned long vbase;
    unsigned long entries[512];
};

struct ptdump 
{
    pid_t processid;
    size_t num_tables;
    size_t num_migrations;
    struct ptdump_table table[PTDUMP_DUMP_SIZE_MAX + 1];
};


/*
 * --------------------------------------------------------------
 * COMMAND: CRC
 * --------------------------------------------------------------
 */

///< IOCTL command param for CRC32 on page tables
#define PTDUMP_IOCTL_CMD_CRC32      0x4

struct ptcrc32
{
    pid_t processid;
    unsigned int pml4;
    unsigned int pdpt;
    unsigned int pdir;
    unsigned int pt;
    unsigned int data;
    unsigned int table;
};

#define PTCRC32_INIT (struct ptcrc32){0,0,0,0,0,0,0}

/*
 * --------------------------------------------------------------
 * COMMAND: RANGES
 * --------------------------------------------------------------
 */


///< IOCTL command param to obtain ranges of page tables
#define PTDUMP_IOCTL_CMD_RANGES     0x5


///< argument struct for the ranges IOCTL command
struct ptranges
{
    pid_t processid;
    unsigned long pml4_base;
    unsigned long pml4_limit;
    unsigned long pt_base;
    unsigned long pt_limit;
    unsigned long code_base;
    unsigned long code_limit;
    unsigned long data_base;
    unsigned long data_limit;
    unsigned long kernel_base;
    unsigned long kernel_limit;
};

#define PTRANGES_INIT (struct ptranges){ \
    .processid = 0,\
    .pml4_base = ULONG_MAX, .pml4_limit = 0, \
    .pt_base = ULONG_MAX, .pt_limit = 0, \
    .code_base = ULONG_MAX, .code_limit = 0, \
    .data_base = ULONG_MAX, .data_limit = 0, \
    .kernel_base = ULONG_MAX, .kernel_limit = 0 }

/*
 * --------------------------------------------------------------
 * COMMAND: NODEMAP
 * --------------------------------------------------------------
 */
#define PTDUMP_IOCTL_NUMA_NODEMAP     0x6

#define MAX_NR_NODES   64
struct numa_node_info
{
    int id;
    unsigned long node_start_pfn;
    unsigned long node_end_pfn;
};

struct nodemap
{
    int nr_nodes;
    struct numa_node_info node[MAX_NR_NODES];
};

/*
 * We save several choices for page tables at the host level in
 * a virtualized system. Use this to configure which one should be
 * dumped.
 * 0 (default): Host Page Tables -- this is the only valid input for unvirtualized case
 * 1 : Extended Page Tables
 */
#define PTDUMP_IOCTL_WHICH_PTABLES      0x7
#define PTDUMP_HOST_PTABLES             0x0
#define PTDUMP_EXT_PTABLES              0x1
#define PTDUMP_SHADOW_PTABLES           0x2
