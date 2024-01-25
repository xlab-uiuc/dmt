# Artifact Evaluation Submission for DMT [ASPLOS '24] 

This repository contains the artifact for reproducing our ASPLOS '24 paper "Direct Memory Translation for Virtualized Clouds". 

# Table of Contents

- [Overview](#overview)
- [Setup](#setup)
- [Running Experiments](#running-experiments)
- [Validation of the main claims](#validation-of-the-main-claims)
- [Known issues](#known-issues)
- [Authors](#authors)
- [License](#license)

# Overview 

### I. Folder Structure

```
root
|---- data                      (Collected evaluation data will be stored here)
|---- datasets                  (Data set used to run the workloads)
|---- eval                      (Various scripts to support the evaluation framework)
|---- linux                     (5.15.127-dmt Linux kernel)
|---- processor                 (Data processing engine to output evaluation data)
|---- scripts                   (Scripts for human execution)
|---- simulator-native          (DynamoRIO simulator to perform native page walk simulation)
|---- simulator-virt            (DynamoRIO simulator to perform nested page walk simulation)
|---- tracer                    (DynamoRIO tracer to capture traces for the simulator)
|---- workloads                 (The workloads we used in the evaluation)
    |---- bin                   (The workload binaries will be generated here)
    |---- btree, canneal, ...   (The source codes of each individual workload)
```



### II. Environment Requirement

Our artifact was implemented based on Linux 5.15.127, and was developed and tested on **Ubuntu 20.04.3 LTS**. Our artifact should run on any Linux distribution. However, porting to other Linux distributions would require some script modifications, especially those in `scripts` folder, which assume the presence of certain tools such as `apt-get`.

Our artifact requires a machine with at least 192 GB of physical memory and 1.5 TB of free disk space if you want to evaluate all three setups (native, virtualized, and nested). Also, the CPUs should have nested paging support enabled (Intel EPT or AMD NPT).



# Setup 

### I. Compile and install the kernel (30-60 min, 50 GB space)

1. Install required dependencies:

  **Note**: Some operations may require root/sudo permission to run.

```bash
$ cd scripts
$ ./install_deps.sh
$ cd ..
```



2. Configure the kernel:

```bash
$ cd linux
$ cp config .config
$ make oldconfig 
```

If you want to use your own configuration file, please make sure your configuration has `CONFIG_KVM=y` and `CONFIG_CMA=y` set.



3. Compile the kernel:

   **Note:** The step below is just for reference only. You may use your favorite way to compile and install the kernel.

```bash
$ make -j`nproc` --load-average=`nproc`
$ make modules -j`nproc` --load-average=`nproc`
$ make scripts_gdb -j`nproc` --load-average=`nproc`
```



4. Compile the `perf` tool:

```bash
$ cd tools/perf
$ make -j`nproc` --load-average=`nproc`
$ cd ../..
```



5. Install the kernel:

   **Note:** We assume your machine use GRUB here. If you use other bootloaders, please make sure to add our kernel to the bootable kernel list.

```bash
$ sudo make modules_install
$ sudo make install
$ sudo update-grub # assumes GRUB
$ cd ..
```



### II. Compile the tracer and simulator (10 min, 2 GB space)

1. Install the required compiler:

   The tracer and simulator can only be compiled under `gcc-7`. This step will switch your system default compiler to `gcc-7`. You may restore the changes afterwards.

```bash
$ sudo apt-get install gcc-7 g++-7
$ sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 10
$ sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-7 10
$ sudo update-alternatives --config gcc # select gcc-7
$ sudo update-alternatives --config g++ # select g++-7
$ gcc --version
gcc (Ubuntu 7.5.0-6ubuntu2) 7.5.0
$ g++ --version
g++ (Ubuntu 7.5.0-6ubuntu2) 7.5.0
```

  If you received a message saying `E: Package 'gcc-7' has no installation candidate`, please run the following commands and try again:

```bash
$ echo 'deb http://archive.ubuntu.com/ubuntu focal main universe' | sudo tee -a /etc/apt/sources.list
$ sudo apt-get update
```



2. Compile the tracer:

```bash
$ cd tracer
$ source ./source.sh
$ ./install.sh
$ cd ..
```



3. Compile the simulator:

```bash
$ cd simulator-native
$ source ./source.sh
$ ./install.sh
$ cd ..
$ cd simulator-virt
$ source ./source.sh
$ ./install.sh
$ cd ..
```



### III. Compile benchmarks (1 min, 60 MB space)

```bash
$ cd workloads
$ make clean-all
$ make all
$ ls bin/
bench_btree_dump  bench_btree_mt  bench_btree_st  bench_canneal_mt
bench_canneal_st  bench_graph500_mt  bench_graph500_st  bench_gups_st
bench_gups_toy  bench_redis_st  bench_stream  bench_xsbench_dump  bench_xsbench_mt
$ cd ..
```



### IV. Compile data processor (1min, 1 MB space)

```bash
$ cd processor
$ dotnet build
$ cd ..
```



### V. Obtain the data set (5 GB space)

Please download the URL and place the file at `datasets/canneal`: https://drive.google.com/file/d/1gQbEGW-Z6oCo0zRIorF1AEo9JJEV5Vob/view?usp=sharing



# Minimal Working Examples

See [minimal.md](minimal.md)



# Evaluation

**Note:** These steps takes significant amount of time to finish due to the amount of data need to be processed by each workload (>100 GB).

**Note:** The time and space estimation below are for single setup. If you want to run all four setups (native, NPT virtualized, SPT virtualized, nested virtualized), the resource consumption will also grow.

### I. Prepare environment (10 min, 5MB space)

Run the following script to prepare the evaluation environment.

```bash
$ cd scripts
$ sudo ./prepare_system.sh
$ cd ..
```



### II. Collect page walk traces (8-12 hours, 200 GB space)

**Note:** Please run this part under the DMT kernel.

**Note:** If you only want to run some workloads, please change `scripts/workloads.sh`

```bash
$ cd scripts
$ sudo ./run_trace.sh
$ cd ..
```



### III. Collect page walk statistics (4-8 hours, 1 GB space)

**Note:** Please run this part under a vanilla Linux kernel.

```bash
$ cd scripts
$ sudo ./run_perf.sh
$ cd ..
```



### IV. Collect host page table (10 min, 5GB space)

**Note:** You only need to perform this step if evaluating virtualized and nested virtualized cases.

**Note:** This step is executed on your host machine, rather than inside the VM

```bash
# Take a note of the PID of the hypervisor, here we use QEMU as an example, the PID is 12345
$ ps aux | grep qemu
libvirt+  12345 90.0 70.0 211028656 203185148 ? Sl   Jan23 338:44 /usr/bin/qemu-system-x86_64 ...
$ cd scripts
$ sudo ./dump_host.sh 12345
# Copy the dumped page table to your guest VM
$ scp ./pt_dump.host your-vm-account@your-vm-ip:/some/remote/directory
$ cd ..
```



### V. Run simulator (4-8 hours, 200 GB temporary space)

**Note:** This part can run under any kernel. For better stability, we recommend vanilla Linux kernel.

**Note:** The simulator may appear unresponsive after some "Initialising a cache with size" lines, sometimes up to one hour. This is normal if the `drmemtrace.trace` file is created and growing, which is located under `data/trace-<workload_name>/drmemtrace.<some_identifier>/` folder.

```bash
# If running native experiment
$ cd scripts
$ sudo ./run_sim_native.sh
$ cd ..

# If running virtualized/nested virtualized experiment
$ cd scripts
$ sudo ./run_sim_virt.sh /path/to/pt_dump.host
$ cd ..
```



### VI. Repeat for other setups

To collect results for NPT virtualized, SPT virtualized or nested environments, please execute the Setup Steps and Evaluation Steps in a virtual machine / nested virtual machine. Note that the runtime may take longer for these two setups due to the virtualization overhead.

**Note:** If you want to switch between shadow paging and nested paging, please do this on your host:

```bash
$ sudo modprobe -r kvm_intel # or kvm_amd if you are using AMD cpu, same below

# To use shadow paging
$ sudo modprobe kvm_intel ept=0 vpid=0 nested=1

# To use nested paging
$ sudo modprobe kvm_intel ept=1 vpid=1 nested=1
```



### VII. Data Summarization

To obtain the final results, please run one of the three following scripts. Note that since nested virtualized performance requires SPT overhead to estimate, you must run EPT and SPT virtualized setups before getting nested virtualization results.

```bash
$ cd scripts
# If evaluated in native environment
$ ./result_native.sh
# If evaluated in NPT/SPT virtualized environment
$ ./result_virt.sh
# If evaluated in nested virtualized environment
$ ./result_nested.sh
```




# Validation of the main claims

### 6.1 DMT Performance over Baseline

**Claims:**

- DMT outperforms the baseline in native, single-level virtualization, and nested virtualization environments.
- pvDMT leads to more substantial performance improvements in virtualized environments.

**Expected results:**

- In native and EPT virtualized environments, DMT should observe positive (> 1.00x) page walk speedup and application speedup against vanilla x86.
- In EPT virtualized environments, pvDMT should observe higher page walk speedup and application speedup than DMT.
- In nested virtualized environments, DMT should observe positive (> 1.00x) application speedup. The page walk speedup may be negative (< 1.00x) due to the use of three-dimensional page walks against vanilla x86.



### 6.2  Comparison with Other Advanced Designs

**Claims:**

- DMT/pvDMT outperforms the state of the arts.

**Expected results:**

- In native and EPT virtualized environments, DMT and pvDMT should observe positive (> 1.00x) page walk speedup and application speedup against other designs.



# Known issues 

1. When there are too many process forks (e.g. running `make`), the DMT kernel may panic due to inconsistent cgroup status

2. The artifact may function improperly on a machine with five-level page table enabled

# Authors

Jiyuan Zhang (University of Illinois Urbana-Champaign)

Weiwei Jia (University of Rhode Island)

Siyuan Chai (University of Illinois Urbana-Champaign)

Peizhe Liu (University of Illinois Urbana-Champaign)

Jongyul Kim (University of Illinois Urbana-Champaign)

Tianyin Xu (University of Illinois Urbana-Champaign)

# License

The DMT Linux kernel is licensed under GNU General Public License v2.0 With Linux Syscall Note.

Other parts of this artifact is licensed under Apache License 2.0.
