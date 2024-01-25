# Minimal working example

**Note:** This example shows EPT virtualized case.

### I. Use the debug workload only

```bash
$ cd scripts
$ cat workloads.sh
#!/usr/bin/env false

WORKLOADS=(
        "debug"
        # "gups"
        # "btree"
        # "redis"
        # "canneal"
        # "graph500"
        # "xsbench"
        # "memcached"
)
```



### II. Prepare environment

```bash
$ sudo ./prepare_system.sh
Faulting all memories on this machine/VM, this may take a while...
/home/ubuntu/dmt-rehearsal/eval /home/ubuntu/dmt-rehearsal/scripts
./prepare_system.sh: line 16:  4030 Killed                  sudo ./faultall
/home/ubuntu/dmt-rehearsal/scripts
/home/ubuntu/dmt-rehearsal/eval/dump_pagetables /home/ubuntu/dmt-rehearsal/scripts
rmmod: ERROR: Module dump_pagetables is not currently loaded
make -C /lib/modules/5.15.127-dmt-demo+/build M=/home/ubuntu/dmt-rehearsal/eval/dump_pagetables modules
make[1]: Entering directory '/home/ubuntu/dmt-demo'
warning: the compiler differs from the one used to build the kernel
  The kernel was built by: gcc (Ubuntu 9.4.0-1ubuntu1~20.04.2) 9.4.0
  You are using:           gcc (Ubuntu 7.5.0-6ubuntu2) 7.5.0
  CC [M]  /home/ubuntu/dmt-rehearsal/eval/dump_pagetables/dump_pagetables.o
  MODPOST /home/ubuntu/dmt-rehearsal/eval/dump_pagetables/Module.symvers
  CC [M]  /home/ubuntu/dmt-rehearsal/eval/dump_pagetables/dump_pagetables.mod.o
  LD [M]  /home/ubuntu/dmt-rehearsal/eval/dump_pagetables/dump_pagetables.ko
  BTF [M] /home/ubuntu/dmt-rehearsal/eval/dump_pagetables/dump_pagetables.ko
make[1]: Leaving directory '/home/ubuntu/dmt-demo'
/home/ubuntu/dmt-rehearsal/scripts
Testing kernel module, this may also take a while...
U1

-----------------------------------------------------------------------

Success - Script execution done

```



### III. Collect page walk traces

```bash
$ sudo ./run_trace.sh
bench_gups_st: no process found
Disable THP...
never
never
Drop OS caches...
3
Checking if kernel module is loaded...
Everything is OK
++++++++++++++++++++++ prehook ++++++++++++++++++++++
++++++++++++++++++++++ starting ++++++++++++++++++++++
APPLICATION             = /home/ubuntu/dmt-rehearsal/workloads/bin/bench_gups_st 16
OUTPUT_DIR              = /home/ubuntu/dmt-rehearsal/data/trace-debug/
ENABLE_FILE             = /tmp/enablement/debug
Start tracer...
INFO: targeting application: "/home/ubuntu/dmt-rehearsal/workloads/bin/bench_gups_st"
INFO: DynamoRIO configuration directory is /root/.dynamorio
INFO: configuring bench_gups_st pid=6323 dr_ops=""
INFO: configuring client "/home/ubuntu/dmt-rehearsal/tracer/build/bin64/../clients/lib64/release/libdrmemtrace.so" ops="-offline -outdir /home/ubuntu/dmt-rehearsal/data/trace-debug/ -verbose 1 -enabler_filename /tmp/enablement/debug -trace_after_instrs 1 -exit_after_tracing 2000000000"
Log directory is /home/ubuntu/dmt-rehearsal/data/trace-debug//drmemtrace.bench_gups_st.06323.8342.dir/raw
/home/ubuntu/dmt-rehearsal/workloads/bin/bench_gups_st 16
prog_argv[0]=/home/ubuntu/dmt-rehearsal/workloads/bin/bench_gups_st
prog_argv[1]=16
<benchmark exec="/home/ubuntu/dmt-rehearsal/workloads/bin/bench_gups_st">
<config>
  <openmp>off</openmp>  <memsize>1G <memsize>  <hugetlb>0</hugetlb>
  <data>0</data>  <pte>0</pte>  <run>0</run>
  <interference>0</interference>
</config>
<run>
<gups tablesize="17179869184"></gups>
<gups table="0x7f92c166c010" tablesize="2147483648"></gups>
++++++++++++++++++++++ posthook ++++++++++++++++++++++
++++++++++++++++++++++ started (PID = 6318) ++++++++++++++++++++++
++++++++++++++++++++++ Programmed File Enablement ++++++++++++++++++++++
signalling readyness to /tmp/enablement/gups_watch
++++++++++++++++++++++ enabled ++++++++++++++++++++++

!!!!***ATTENTION***!!!! 25-01-2024 05:21:35     Starting recording a trace on 6323

U6323
U6323
U6323
U6323
U6323
U6323
U6323
U6323
U6323
U6323
U6323
U6323
U6323
U6323
U6323
U6323
U6323
Tracer reached the target of 2000000000 and will dump the page table by executing cat /proc/page_tables > /home/ubuntu/dmt-rehearsal/data/trace-debug//pt_dump_raw ...Done
Exiting process after ~2000004067 references.
drmemtrace exiting process 6323; traced 2000004068 references.
Picked input: /home/ubuntu/dmt-rehearsal/data/trace-debug/pt_dump_raw.9 (222396379) bytes

-----------------------------------------------------------------------

Success - Script execution done

```



### IV. Collect page walk statistics

```bash
$ sudo ./run_perf.sh
always
bench_gups_st: no process found
++++++++++++++++++++++ prehook ++++++++++++++++++++++
++++++++++++++++++++++ starting ++++++++++++++++++++++
++++++++++++++++++++++ posthook ++++++++++++++++++++++

-----------------------------------------------------------------------

Success - Script execution done
never
bench_gups_st: no process found
++++++++++++++++++++++ prehook ++++++++++++++++++++++
++++++++++++++++++++++ starting ++++++++++++++++++++++
++++++++++++++++++++++ posthook ++++++++++++++++++++++

-----------------------------------------------------------------------

Success - Script execution done
```



### V. Collect host page table

```bash
$ ps aux | grep qemu
libvirt+  563806  161 76.8 211012264 202563656 ? Sl   Jan23 1160:19 /usr/bin/qemu-system-x86_64 -name guest=nested,debug-threads=on -S -object secret, [... 2 more matches]
$ sudo ./dump_host.sh 563806
Dumping hypervisor page tables
V6715
Processing page table data
The page table dump is ready at /home/jz/dmt-rehearsal/scripts/pt_dump.host

-----------------------------------------------------------------------

Success - Script execution done
$ scp ./pt_dump.host ubuntu@nested:/home/ubuntu/dmt-rehearsal/
pt_dump.host                                                           100% 2626MB 277.5MB/s   00:09
```



### VI. Run simulator

```bash
$ sudo ./run_sim_virt.sh /home/ubuntu/dmt-rehearsal/pt_dump.host
Initialising a TLB with size: 128
Initialising a TLB with size: 64
Initialising a TLB with size: 1536
Initialising a TLB with size: 128
Initialising a TLB with size: 64
Initialising a TLB with size: 1536
Initialising a TLB with size: 128
Initialising a TLB with size: 64
Initialising a TLB with size: 1536
Initialising a TLB with size: 128
Initialising a TLB with size: 64
Initialising a TLB with size: 1536
Initialising a TLB with size: 128
Initialising a TLB with size: 64
Initialising a TLB with size: 1536
Initialising a TLB with size: 128
Initialising a TLB with size: 64
Initialising a TLB with size: 1536
Initialising a TLB with size: 128
Initialising a TLB with size: 64
Initialising a TLB with size: 1536
Initialising a TLB with size: 128
Initialising a TLB with size: 64
Initialising a TLB with size: 1536
Initialising a TLB with size: 128
Initialising a TLB with size: 64
Initialising a TLB with size: 1536
Initialising a TLB with size: 128
Initialising a TLB with size: 64
Initialising a TLB with size: 1536
Initialising a TLB with size: 128
Initialising a TLB with size: 64
Initialising a TLB with size: 1536
Initialising a TLB with size: 128
Initialising a TLB with size: 64
Initialising a TLB with size: 1536
Initialising a TLB with size: 128
Initialising a TLB with size: 64
Initialising a TLB with size: 1536
Initialising a TLB with size: 128
Initialising a TLB with size: 64
Initialising a TLB with size: 1536
Initialising a TLB with size: 128
Initialising a TLB with size: 64
Initialising a TLB with size: 1536
Initialising a TLB with size: 128
Initialising a TLB with size: 64
Initialising a TLB with size: 1536
Initialising a TLB with size: 128
Initialising a TLB with size: 64
Initialising a TLB with size: 1536
Initialising a TLB with size: 128
Initialising a TLB with size: 64
Initialising a TLB with size: 1536
Initialising a TLB with size: 128
Initialising a TLB with size: 64
Initialising a TLB with size: 1536
Initialising a TLB with size: 128
Initialising a TLB with size: 64
Initialising a TLB with size: 1536
Initialising PT from file: /home/ubuntu/dmt-rehearsal/scripts/../data/trace-debug/pt_dump
Loading page table with 4196182 PT entries...
Loaded 4196182 unique PT entries.
Initialising VT_PT from file: /home/ubuntu/dmt-rehearsal/scripts/../data/trace-debug/pt_dump.host
Loading host_page table with 50387004 PT entries...
Loaded 50387004 unique PT entries.
Initialising a cache with size: 23068672 with assoc: 11 with line size: 64
Initialising a cache with size: 1048576 with assoc: 16 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 1048576 with assoc: 16 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 1048576 with assoc: 16 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 1048576 with assoc: 16 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 1048576 with assoc: 16 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 1048576 with assoc: 16 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 1048576 with assoc: 16 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 1048576 with assoc: 16 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 1048576 with assoc: 16 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 1048576 with assoc: 16 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 1048576 with assoc: 16 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 1048576 with assoc: 16 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 1048576 with assoc: 16 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 1048576 with assoc: 16 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 1048576 with assoc: 16 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 1048576 with assoc: 16 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 1048576 with assoc: 16 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 1048576 with assoc: 16 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 1048576 with assoc: 16 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 1048576 with assoc: 16 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 32768 with assoc: 8 with line size: 64
Initialising a cache with size: 16 with assoc: 1 with line size: 8
Initialising a cache with size: 32 with assoc: 4 with line size: 8
Initialising a cache with size: 256 with assoc: 4 with line size: 8
Initialising a cache with size: 16 with assoc: 1 with line size: 8
Initialising a cache with size: 32 with assoc: 4 with line size: 8
Initialising a cache with size: 256 with assoc: 4 with line size: 8
Heartbeat. 4194304 references processed.
TLB simulation results:
Core #0 (1 thread(s))
  TLB-L1I stats:
    Hits:                        3,101,160
    Misses:                              1
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Hits PT level     5:                   0
    Misses PT level   5:                   0
    Hits PT level     6:                   0
    Misses PT level   6:                   0
    Miss rate:                        0.00%
  TLB-L1D stats:
    Hits:                          729,020
    Misses:                        363,498
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Hits PT level     5:                   0
    Misses PT level   5:                   0
    Hits PT level     6:                   0
    Misses PT level   6:                   0
    Miss rate:                       33.27%
  TLB-LL stats:
    Hits:                              506
    Misses:                        362,993
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Hits PT level     5:                   0
    Misses PT level   5:                   0
    Hits PT level     6:                   0
    Misses PT level   6:                   0
    Local miss rate:                 99.86%
    Child hits:                  3,830,180
    Total miss rate:                  8.66%
Core #1 (0 thread(s))
Core #2 (0 thread(s))
Core #3 (0 thread(s))
Core #4 (0 thread(s))
Core #5 (0 thread(s))
Core #6 (0 thread(s))
Core #7 (0 thread(s))
Core #8 (0 thread(s))
Core #9 (0 thread(s))
Core #10 (0 thread(s))
Core #11 (0 thread(s))
Core #12 (0 thread(s))
Core #13 (0 thread(s))
...
```



### VII. Data Summarization

```bash
$ ./result_virt.sh
---------------------------------------------------------
Result for workload debug in NPT or SPT virtualized case

Workload Runtime: Base 57.28 sec / Huge 33.60 sec
Page Walk Overhead: Base 91.8% / Huge 80.0%
Average Walk Latency: Base 184.5 cycles / Huge 112.2 cycles

Base Page Result ----------------
Page Walk Speedup of Vanilla: 1.00x
Page Walk Speedup of FPT: 1.12x
Page Walk Speedup of ECPT: 1.13x
Page Walk Speedup of ASAP: 1.20x
Page Walk Speedup of DMT: 1.22x

Application Speedup of Vanilla: 1.00x
Application Speedup of FPT: 1.10x
Application Speedup of ECPT: 1.10x
Application Speedup of ASAP: 1.18x
Application Speedup of DMT: 1.20x

Huge Page Result ----------------
Page Walk Speedup of Vanilla: 1.00x
Page Walk Speedup of FPT: 1.20x
Page Walk Speedup of ECPT: 1.13x
Page Walk Speedup of ASAP: 1.38x
Page Walk Speedup of DMT: 1.41x

Application Speedup of Vanilla: 1.00x
Application Speedup of FPT: 1.17x
Application Speedup of ECPT: 1.11x
Application Speedup of ASAP: 1.34x
Application Speedup of DMT: 1.35x



-----------------------------------------------------------------------

Success - Script execution done
```

