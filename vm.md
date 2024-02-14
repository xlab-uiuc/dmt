# Deployment of VM

### I. Check if virtualization is supported

> [!Important]
>
> If any of the below checking step failed, you should use another environment for evaluation.

```shell
# Check for hardware-assisted virtualization capability, should see a number greater than zero
$ sudo egrep -c '(vmx|svm)' /proc/cpuinfo
32

# Check for KVM readyness, should see the exact output below
$ sudo apt install cpu-checker
$ sudo kvm-ok
INFO: /dev/kvm exists
KVM acceleration can be used

# Enable nested on current session
$ sudo modprobe -r kvm_intel
$ sudo modprobe kvm_intel nested=1

# Check for nested virtualization capability. If it is Y, please go to Step 2 directly
$ sudo cat /sys/module/kvm_intel/parameters/nested
Y
```



### II. Install dependencies

```shell
# Install kvm and tools
$ sudo apt install -y qemu-kvm virt-manager libvirt-daemon-system virtinst libvirt-clients bridge-utils

# Enable virtualization service
$ sudo systemctl enable --now libvirtd
$ sudo systemctl start libvirtd
$ sudo systemctl status libvirtd
$ sudo usermod -aG kvm $USER
$ sudo usermod -aG libvirt $USER

# Prepare a list of hardware counters (for cross reference in Step IV)
$ perf list | grep dtlb_load_misses
  dtlb_load_misses.stlb_hit
  dtlb_load_misses.walk_active
  dtlb_load_misses.walk_completed
  dtlb_load_misses.walk_completed_2m_4m
  dtlb_load_misses.walk_completed_4k
  dtlb_load_misses.walk_pending
```



### III. Create VM

> [!Note]
>
> You need two console session and VNC connection for this step

> [!Tip]
>
> If you are connecting to the server via SSH, you may use [SSH Port Forwarding](https://phoenixnap.com/kb/ssh-port-forwarding) to connect to remote VNC session

> [!Tip]
>
> For VNC, `:0` means TCP port `5900`, `:1` means TCP port `5901`, `:N` means TCP port `5900 + N`

---

> [!Note]
>
> This step is performed inside console A

```shell
# Get VM Image
# you may place the image to anywhere you want, but please also change the path in Setup VM command
$ cd /var/lib/libvirt/boot/
$ sudo wget -O ubuntu2004.iso https://mirrors.edge.kernel.org/ubuntu-releases/20.04/ubuntu-20.04.6-live-server-amd64.iso

# Setup a VM with 80 vCPU core, 245760 MB (240 GB) memory and 600 GB disk
# You may adjust the numbers (--vcpus, --ram, and --disk) to customize your machine
$ sudo virt-install \
    --virt-type=kvm \
    --name "<YOUR VM NAME HERE>" \
    --ram 245760 \
    --vcpus=80 \
    --os-variant=ubuntu20.04 \
    --virt-type=kvm \
    --hvm \
    --cdrom=/var/lib/libvirt/boot/ubuntu2004.iso \
    --network=default,model=virtio \
    --graphics vnc \
    --disk path=/var/lib/libvirt/images/nested.qcow2,size=600,bus=virtio,format=qcow2
    
# Now switch to console B
```
---

> [!Note]
>
> This step is performed inside console B

```shell
# Create a VNC session to perform system installation
$ sudo virsh vncdisplay "<YOUR VM NAME HERE>"
:0

# Now you can connect VNC client to perform OS installation
# Once done, please switch back to console A and close console B
```
---

> [!Note]
>
> This step is performed inside console A

```shell
# Shutdown the VM
$ sudo virsh shutdown "<YOUR VM NAME HERE>"

# Change configuration
$ sudo virsh edit "<YOUR VM NAME HERE>"
# Please make sure <vcpu> tag has attribute placement='static' (e.g. <vcpu placement='static'>80</vcpu>)
# Please make sure <cpu> tag has attribute mode='host-passthrough' (e.g. <cpu mode='host-passthrough' check='partial'/>)

# Start the VM
$ sudo virsh start "<YOUR VM NAME HERE>"

# Get the IP of the VM
$ sudo virsh net-dhcp-leases default

# Once done, you can now connect to the VM via SSH. VNC is no longer needed.
# Note that you may need to proxy the SSH connection via the host machine because VM is not open to public Internet by default.
```



### IV. Inside VM

> [!Note]
>
> This step is performed inside the VM you just installed

```shell
# Check hardware spec is as intended
$ lscpu
...
CPU(s):                  80
  On-line CPU(s) list:   0-79
...

$ free -g
      total ...
Mem:    240 ...
...

$ lsblk
NAME   MAJ:MIN RM   SIZE RO TYPE MOUNTPOINTS
...
vda    252:0    0   600G  0 disk
...

# Perform basic environment setup
$ sudo apt-get update
$ sudo apt-get install -y git vim wget grep linux-tools-`uname -r`

# Check if hardware counter is available inside the VM
# Please make sure the listed output is the same as your execution result from Step II
# Below is an example output. If your result is different, please modify the scripts to fit your hardware
$ perf list | grep dtlb_load_misses
  dtlb_load_misses.stlb_hit
  dtlb_load_misses.walk_active
  dtlb_load_misses.walk_completed
  dtlb_load_misses.walk_completed_2m_4m
  dtlb_load_misses.walk_completed_4k
  dtlb_load_misses.walk_pending

# Clone the DMT artifact
$ git clone https://github.com/xlab-uiuc/dmt.git

# Now please follow the artifact setup guide
```

