/*
 * Debug helper to dump the current kernel nested EPT pagetables of the system
 * so that we can see what the various memory ranges are set to.
 *
 * (C) Copyright 2024 Jiyuan Zhang
 *
 * Author: Jiyuan Zhang <jiyuanz3@illinois.edu>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 */

#include <linux/proc_fs.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/seq_file.h>
#include <linux/sort.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/sched/mm.h>
#include <linux/security.h>
#include <linux/kvm_host.h>
#include <linux/ptrace.h>
#include <linux/kprobes.h>

#include <asm/pgtable.h>
#include <asm/pgtable_64.h>

//
// Globals
//

#define is_default_mode() (selected_mode == '0')
#define is_vmroot_mode() (selected_mode == 'V')
#define is_userspace_mode() (selected_mode == 'U')

// From arch/x86/kvm/mmu.h
#define PT64_ROOT_5LEVEL 5
#define PT64_ROOT_4LEVEL 4
#define PT32_ROOT_LEVEL 2
#define PT32E_ROOT_LEVEL 3

// For all modes
static pid_t selected_pid = 1;
static char selected_mode = '0';

// For vmroot mode
static pid_t vmroot_pid = 1;
static ulong vmroot_pgtbl = 0;
static bool vmroot_direct = true;
static int vmroot_type = PT64_ROOT_4LEVEL;
static int vmroot_maxphys = 47;

static long unsigned int cr3_pfn(pgd_t *root)
{
	void *vcr3 = root;
	return __pa(vcr3) >> PAGE_SHIFT;
}

static void print_entry(struct seq_file *s, pgd_t *root, ulong va, ulong pgdp, ulong p4dp, ulong pudp, ulong pmdp, ulong ptep, bool huge)
{
	if (!ptep)
		return;

	seq_printf(s, "%016lx,%lx,%lx,%lx,%lx,%lx\n", va, cr3_pfn(root), p4dp, pudp, pmdp, ptep);
}

//
// VM EPT Table Structure
//

// Intel SDM Vol. 3C Pg. 28-12
// Figure 28-1. Formats of EPTP and EPT Paging-Structure Entries

#define EPT_PHYS_MAX ((1UL << vmroot_maxphys) - 1)
#define EPT_PFN_MASK (EPT_PHYS_MAX & (~((1 << 12) - 1)))

#define ept_functions(type)												\
static bool ept_##type##_none(type##_t entry)							\
{																		\
	unsigned long val = entry.type;										\
	return (val & 0x7) == 0;											\
}																		\
																		\
static bool ept_##type##_bad(type##_t entry)							\
{																		\
	return false;														\
}																		\
																		\
static unsigned long ept_##type##_pfn(type##_t entry)					\
{																		\
	unsigned long val = entry.type;										\
	return (val & EPT_PFN_MASK) >> 12;									\
}																		\
																		\
static __maybe_unused bool ept_##type##_leaf(type##_t entry)			\
{																		\
	unsigned long val = entry.type;										\
	return !!(val & 0x80);												\
}																		\
																		\
static __maybe_unused void* ept_##type##_page_vaddr(type##_t entry)		\
{																		\
	unsigned long val = entry.type;										\
	return __va(val & EPT_PFN_MASK);									\
}																		\

ept_functions(pgd)
ept_functions(p4d)
ept_functions(pud)
ept_functions(pmd)
ept_functions(pte)

// It should be safe to assume xxx_index stays common
#define ept_offset(from, to, ent, addr) (((to##_t*) ept_##from##_page_vaddr(ent)) + to##_index(addr))

static inline p4d_t *ept_p4d_offset(pgd_t *pgd, unsigned long address)
{
	if (vmroot_type != PT64_ROOT_5LEVEL)
		return (p4d_t *)pgd;
	return (p4d_t *)ept_pgd_page_vaddr(*pgd) + p4d_index(address);
}

//
// VM EPT Table Walker
//

static void handle_ept_page(struct seq_file *s, pgd_t *root, ulong *paddr, ulong pgdp, ulong p4dp, ulong pudp, ulong pmdp, ulong ptep, int level)
{
	ulong addr = *paddr;

	switch(level) {
	case 0: {
		p4dp = pgdp + p4d_index(addr) * (P4D_SIZE / PAGE_SIZE);
		fallthrough;
	}

	case 1: {
		pudp = p4dp + pud_index(addr) * (PUD_SIZE / PAGE_SIZE);
		fallthrough;
	}

	case 2: {
		pmdp = pudp + pmd_index(addr) * (PMD_SIZE / PAGE_SIZE);
		fallthrough;
	}

	case 3: {
		ptep = pmdp + pte_index(addr) * (PAGE_SIZE / PAGE_SIZE);
		print_entry(s, root, addr, pgdp, p4dp, pudp, pmdp, ptep, true);
		*paddr += PAGE_SIZE;
		return;
	}

	default: {
		WARN_ONCE(1, "Invalid huge level\n");
		return;
	}
	}
}

static void ept_table_walker(struct seq_file *s, unsigned long *paddr, pgd_t *root)
{
	unsigned long addr = *paddr;
	unsigned long step = PAGE_SIZE;
	unsigned long pgdp = 0, p4dp = 0, pudp = 0, pmdp = 0, ptep = 0;

	pgd_t *pgd = NULL;
	p4d_t *p4d = NULL;
	pud_t *pud = NULL;
	pmd_t *pmd = NULL;
	pte_t *pte = NULL;

	pgd = root + pgd_index(addr);

	if (pgd && !ept_pgd_none(*pgd)) {
		pgdp = ept_pgd_pfn(*pgd);

		if (ept_pgd_leaf(*pgd))
			return handle_ept_page(s, root, paddr, pgdp, p4dp, pudp, pmdp, ptep, 0);

		if (!ept_pgd_bad(*pgd))
			p4d = ept_p4d_offset(pgd, addr);
	}
	if (p4d && !ept_p4d_none(*p4d)) {
		p4dp = ept_p4d_pfn(*p4d);

		if (ept_p4d_leaf(*p4d))
			return handle_ept_page(s, root, paddr, pgdp, p4dp, pudp, pmdp, ptep, 1);

		if (!ept_p4d_bad(*p4d))
			pud = ept_offset(p4d, pud, *p4d, addr);
	}
	if (pud && !ept_pud_none(*pud)) {
		pudp = ept_pud_pfn(*pud);

		if (ept_pud_leaf(*pud))
			return handle_ept_page(s, root, paddr, pgdp, p4dp, pudp, pmdp, ptep, 2);

		if (!ept_pud_bad(*pud))
			pmd = ept_offset(pud, pmd, *pud, addr);
	}
	if (pmd && !ept_pmd_none(*pmd)) {
		pmdp = ept_pmd_pfn(*pmd);

		if (ept_pmd_leaf(*pmd))
			return handle_ept_page(s, root, paddr, pgdp, p4dp, pudp, pmdp, ptep, 3);

		if (!ept_pmd_bad(*pmd))
			pte = ept_offset(pmd, pte, *pmd, addr);
	}
	if (pte && !ept_pte_none(*pte)) {
		if (!ept_pte_bad(*pte))
			ptep = ept_pte_pfn(*pte);
	}

	if (!ptep) { step = PAGE_SIZE; }
	if (!pmdp) { step = PMD_SIZE; }
	if (!pudp) { step = PUD_SIZE; }
	if (!p4dp) { step = P4D_SIZE; }
	if (!pgdp) { step = PGDIR_SIZE; }

	print_entry(s, root, addr, pgdp, p4dp, pudp, pmdp, ptep, false);

	*paddr += step;
}

//
// Process Table Walker
//

static void handle_normal_page(struct seq_file *s, pgd_t *root, ulong *paddr, ulong pgdp, ulong p4dp, ulong pudp, ulong pmdp, ulong ptep, int level)
{
	ulong addr = *paddr;

	switch(level) {
	case 0: {
		p4dp = pgdp + p4d_index(addr) * (P4D_SIZE / PAGE_SIZE);
		fallthrough;
	}

	case 1: {
		pudp = p4dp + pud_index(addr) * (PUD_SIZE / PAGE_SIZE);
		fallthrough;
	}

	case 2: {
		pmdp = pudp + pmd_index(addr) * (PMD_SIZE / PAGE_SIZE);
		fallthrough;
	}

	case 3: {
		ptep = pmdp + pte_index(addr) * (PAGE_SIZE / PAGE_SIZE);
		print_entry(s, root, addr, pgdp, p4dp, pudp, pmdp, ptep, true);
		*paddr += PAGE_SIZE;
		return;
	}

	default: {
		WARN_ONCE(1, "Invalid huge level\n");
		return;
	}
	}
}

static void normal_table_walker(struct seq_file *s, unsigned long *paddr, pgd_t *root)
{
	unsigned long addr = *paddr;
	unsigned long step = PAGE_SIZE;
	unsigned long pgdp = 0, p4dp = 0, pudp = 0, pmdp = 0, ptep = 0;

	pgd_t *pgd = NULL;
	p4d_t *p4d = NULL;
	pud_t *pud = NULL;
	pmd_t *pmd = NULL;
	pte_t *pte = NULL;

	pgd = pgd_offset_pgd(root, addr);

	if (pgd && !pgd_none(*pgd)) {
		pgdp = pgd_pfn(*pgd);

		if (pgd_leaf(*pgd))
			return handle_normal_page(s, root, paddr, pgdp, p4dp, pudp, pmdp, ptep, 0);

		if (!pgd_bad(*pgd))
			p4d = p4d_offset(pgd, addr);
	}
	if (p4d && !p4d_none(*p4d)) {
		p4dp = p4d_pfn(*p4d);

		if (p4d_leaf(*p4d))
			return handle_normal_page(s, root, paddr, pgdp, p4dp, pudp, pmdp, ptep, 1);

		if (!p4d_bad(*p4d))
			pud = pud_offset(p4d, addr);
	}
	if (pud && !pud_none(*pud)) {
		pudp = pud_pfn(*pud);

		if (pud_leaf(*pud))
			return handle_normal_page(s, root, paddr, pgdp, p4dp, pudp, pmdp, ptep, 2);

		if (!pud_bad(*pud))
			pmd = pmd_offset(pud, addr);
	}
	if (pmd && !pmd_none(*pmd)) {
		pmdp = pmd_pfn(*pmd);

		if (pmd_leaf(*pmd))
			return handle_normal_page(s, root, paddr, pgdp, p4dp, pudp, pmdp, ptep, 3);

		if (!pmd_bad(*pmd))
			pte = pte_offset_map(pmd, addr);
	}
	if (pte && !pte_none(*pte)) {
		if (pte_present(*pte))
			ptep = pte_pfn(*pte);
		pte_unmap(pte);
	}

	if (!ptep) { step = PAGE_SIZE; }
	if (!pmdp) { step = PMD_SIZE; }
	if (!pudp) { step = PUD_SIZE; }
	if (!p4dp) { step = P4D_SIZE; }
	if (!pgdp) { step = PGDIR_SIZE; }

	print_entry(s, root, addr, pgdp, p4dp, pudp, pmdp, ptep, false);

	*paddr += step;
}

//
// Proc File
//

static struct mm_struct *get_mm_from_pid(pid_t pid)
{
	struct task_struct *task;
	struct mm_struct *mm;

	task = pid_task(find_vpid(pid), PIDTYPE_PID);
	if (!task) {
		return NULL;
	}

	mm = get_task_mm(task);

	if (!mm) {
		return NULL;
	}

	return mm;
}

static int pt_seq_show(struct seq_file *s, void *v)
{
	ulong *spos = v;

	if (!*spos) {
		pr_warn(
			"Dumping %s page table for %s %d\n", 
			is_userspace_mode() ? "user" : "all",
			is_vmroot_mode() ? "VM" : "process",
			selected_pid
		);

		seq_printf(s, "%16d\n", selected_pid);
	}

	// Only EPT needs special walker, SPT does not
	if (is_vmroot_mode() && vmroot_direct) {
		// Not captured yet
		if (vmroot_pid != selected_pid) {
			seq_printf(s, "VM enter event not fired, please wait a while.\n");
			return -EAGAIN;
		}

		// Thank you, Intel
		if (vmroot_type != PT64_ROOT_4LEVEL && vmroot_type != PT64_ROOT_5LEVEL) {
			seq_printf(s, "Legacy VM mapping mode is not supported.\n");
			return -EINVAL;
		}

		ept_table_walker(s, spos, __va(vmroot_pgtbl));
	}
	else {
		struct mm_struct *mm = get_mm_from_pid(selected_pid);

		if (!mm) {
			seq_printf(s, "Cannot access mm_struct for %d\n", selected_pid);
			return -EINVAL;
		}

		normal_table_walker(s, spos, mm->pgd);

		mmput(mm);
	}

	return 0;
}

static void *pt_seq_start(struct seq_file *s, loff_t *pos)
{
	loff_t *spos;

	// Do not restart terminated sequence
	if (*pos == -1UL)
		return NULL;

	spos = kmalloc(sizeof(loff_t), GFP_KERNEL);
	if (!spos)
		return NULL;

	*spos = *pos;
	return spos;
}

static void *pt_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	loff_t *spos = v;
	ulong sposv = *spos;
	ulong posv = *pos;

	// Stall or overflow
	if (sposv <= posv)
		goto terminate;

	// No need to go beyond
	if (is_vmroot_mode() && sposv >= EPT_PHYS_MAX)
		goto terminate;

	// Go past user space
	if (sposv >= TASK_SIZE_MAX) {
		if (is_userspace_mode())
			goto terminate;

		// Jump to kernel space
		if (sposv < PAGE_OFFSET)
			*spos = PAGE_OFFSET;
	}

	*pos = *spos;

	return spos;

terminate:
	*spos = -1UL;
	*pos = -1UL;
	return NULL;
}

static void pt_seq_stop(struct seq_file *s, void *v)
{
	kfree(v);
}

ssize_t pt_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	char mybuf[65];
	char *myptr = mybuf;
	size_t mylen = sizeof(mybuf) - 1;

	if (count < mylen) {
		mylen = count;
	}

	if (!mylen)
		return count;

	if (copy_from_user(myptr, buf, mylen))
		return -1;

	myptr[mylen] = '\0';

	selected_mode = myptr[0];
	if (selected_mode < '0' || '9' < selected_mode) {
		myptr++;
		mylen--;

		if (!mylen)
			return count;
	}
	else {
		selected_mode = '0';
	}

	if (kstrtoint(myptr, 10, &selected_pid)) {
		pr_warn("Unable to parse PID %s\n", myptr);
		selected_pid = current->pid;
	}

	return count;
}

static struct seq_operations pt_seq_ops = {
	.start = pt_seq_start,
	.next  = pt_seq_next,
	.stop  = pt_seq_stop,
	.show  = pt_seq_show,
};

static int pt_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &pt_seq_ops);
};

static struct proc_ops pt_file_ops = {
	.proc_open    = pt_open,
	.proc_read    = seq_read,
	.proc_write   = pt_write,
	.proc_lseek   = seq_lseek,
	.proc_release = seq_release
};

//
// Probe KVM
//

static int extract_tdp_root(struct kprobe *p, struct pt_regs *regs)
{
	struct kvm_vcpu *cpu = (void *) regs_get_kernel_argument(regs, 0);
	struct kvm_mmu *mmu = cpu->arch.mmu;

	if (cpu->kvm->userspace_pid == selected_pid) {
		pid_t oldpid = vmroot_pid;

		vmroot_pid = cpu->kvm->userspace_pid;
		vmroot_pgtbl = mmu->root_hpa;
		vmroot_type = mmu->shadow_root_level;
		vmroot_direct = mmu->direct_map;
		vmroot_maxphys = cpu->arch.maxphyaddr;

		if (oldpid != vmroot_pid) {
			pr_warn("Got VM %d: Type %d %s page table with root 0x%lx mapping %d bit phys addr.\n",
				vmroot_pid, vmroot_type, vmroot_direct ? "nested" : "shadow", vmroot_pgtbl, vmroot_maxphys);
		}
	}

	return 0;
}

static struct kprobe kp = {
	.symbol_name = "vcpu_load",
	.pre_handler = extract_tdp_root,
};

//
// Module Manager
//

int init_module(void)
{
	int ret;

	struct proc_dir_entry *pt_file = proc_create("page_tables", 0666, NULL, &pt_file_ops);
	if (!pt_file)
		return -ENOMEM;

	ret = register_kprobe(&kp);
	if (ret < 0) {
		pr_err("register_kprobe failed, returned %d\n", ret);
		remove_proc_entry("page_tables", NULL);
		return ret;
	}

	return 0;
}

void cleanup_module(void)
{
	unregister_kprobe(&kp);
	remove_proc_entry("page_tables", NULL);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jiyuan Zhang <jiyuanz3@illinois.edu>");
MODULE_DESCRIPTION("Kernel debugging helper that dumps nested EPT pagetables");
