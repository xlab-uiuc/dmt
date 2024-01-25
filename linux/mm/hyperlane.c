/**
 * DMT/Hyperlane Implementation
 *
 * Historically, DMT Allocator was called Hyperlane.
 */
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/gfp.h>
#include <linux/printk.h>
#include <linux/slab.h>
#include <linux/hyperlane.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/list.h>
#include <linux/pagewalk.h>
#include <linux/rcupdate.h>
#include <linux/smp.h>
#include <linux/swap.h>
#include <linux/migrate.h>
#include <linux/hugetlb.h>
#include <asm/pgalloc.h>

/* Runtime Constants */
#define HPL_POOL_FREE false
#define HPL_POOL_TAKEN true
#define HPL_EXT_REAR 0
#define HPL_EXT_FRONT 1
#define HPL_FOUR_LVL_VMA_IGR 0x7FFFFFFFF000UL
#define HPL_FIVE_LVL_VMA_IGR 0xFFFFFFFFFFF000UL
#define HPL_PROC_DIR_PERMISSION 0777
#define HPL_PROC_ENTRY_PERMISSION 0666
#define HPL_HC_MODE_ALLOC TEA_MODE(0, 0, 0)
#define HPL_HC_MODE_FREE TEA_MODE(0, 1, 0)
#define HPL_CHECK_NO_EXPAND false
#define HPL_CHECK_ALLOW_EXPAND true

/* Option Flags -- Modify if needed */
#define HPL_MERGE_CHECK 1
#define HPL_VMA_IGR_CHECK 1
#define HPL_SEG_INCLUSION_CHECK 0
#define HPL_ALLOW_EXPAND 1
#define HPL_TEST_CS423_ENV 0
#define HPL_TEST_EXPAND 0
#define HPL_TEST_PRINT_TIME false

/* Parameters -- Modify if needed */
#define HPL_VMA_THRESHOLD 64
#define HPL_MERGE_THRESHOLD 98
#define HPL_BIG_TEA_LEN (65536UL * 2)
#define HPL_PROC_DIR "dmt"
#define HPL_PROC_ENTRY "big_tea"
#define HPL_HC_GPA_BEGIN 0x10000000000UL
#define HPL_HC_PGCNT max_pfn * 10 / 100

/* Macros */
#define HPL_VADDR_TO_VPMD(addr) (addr >> PMD_SHIFT)
#define HPL_VPMD_TO_VADDR(vpmd) (vpmd << PMD_SHIFT)
#define HPL_VADDR_RANGE_TO_SIZE(end, start) ((end - start) / 1048576UL)
#define HPL_MERGE_OCCUPATION(newlen, vmalen, oldlen) ((vmalen + oldlen) * 100UL / newlen)
#define HPL_SEG_I_TO_SEG(mm, seg_i) (mm->hpl_info.segments[seg_i - 1])

/* Static Variables */
static struct kmem_cache *hpl_seg_cachep = NULL;
static struct proc_dir_entry* hpl_proc_dir = NULL;
static struct proc_dir_entry* hpl_proc_entry = NULL;
static struct list_head hpl_big_tea_list_head;
static struct kmem_cache *hpl_big_tea_cache = NULL;
static unsigned long hpl_hc_gva = 0;
static unsigned long hpl_hc_gpa = HPL_HC_GPA_BEGIN;
static struct hpl_segment *hpl_walk_seg_migrating;
static struct hpl_segment hpl_walk_seg_new;

/* Static Function Implementations */
/* Proc file system -- CS 423 good old days*/
/* Ref: https://www.kernel.org/doc/html/v5.8/filesystems/seq_file.html */

struct hpl_big_tea {
	struct list_head head;
	char *comm;
};

static void* hpl_proc_seq_start(struct seq_file *file, loff_t *pos) {
    return seq_list_start(&hpl_big_tea_list_head, *pos);
}

static void* hpl_proc_seq_next(struct seq_file *file, void *v, loff_t *pos)
{
    return seq_list_next(v, &hpl_big_tea_list_head, pos);
}

static void hpl_proc_seq_stop(struct seq_file *file, void *v) {
    return;
}

static int hpl_proc_seq_show(struct seq_file *file, void *v) {
    struct hpl_big_tea *list;
	list = list_entry(v, struct hpl_big_tea, head);
	seq_puts(file, list->comm);
	seq_puts(file, "\n");
	return 0;
}

static struct seq_operations hpl_proc_seq_ops = {
    .start = hpl_proc_seq_start,
    .next  = hpl_proc_seq_next,
    .stop  = hpl_proc_seq_stop,
    .show  = hpl_proc_seq_show
};

static int hpl_proc_open(struct inode *inode, struct file *file)
{
    return seq_open(file, &hpl_proc_seq_ops);
}

static ssize_t hpl_proc_write(struct file *file, const char __user *user_buf, size_t size, loff_t *loff) {
	struct list_head *curr_head, *tmp;
    struct hpl_big_tea *list;
	char *from_user, *start, *end;
	char op;
	start = NULL;
	end = NULL;

	if (!hpl_big_tea_cache) {
		pr_err("DMT: hpl_proc_write: Cache is not ready\n");
		return -EFAULT;
	}

	/* Copy from user */
	from_user = kmalloc(size + 1, GFP_KERNEL);
	if (!from_user) {
		pr_err("DMT: hpl_proc_write: Unable to allocate from_user buffer, size=%ld\n", size + 1);
		return -EFAULT;
	}
	memset(from_user, 0, size + 1);
	if (copy_from_user(from_user, user_buf, size)) {
		pr_err("DMT: hpl_proc_write: Unable to copy from user, size=%ld\n", size);
		return -EFAULT;
	}
	if (strlen(from_user) < 3) {  /* Minimal length: op, space, comm[0] = 3 */
		pr_warn("DMT: hpl_proc_write: Illegal format or empty comm list\n");
		return size;
	}
	if (from_user[strlen(from_user) - 1] == '\n') {  /* Prevent auto new line */
		from_user[strlen(from_user) - 1] = '\0';
	}
	
	/* Parse operation and comm list */
	/**
	 * Supported Operation:
	 * R [Comm list]
	 * D [Comm list]
	 *
	 * Comm list should be separated by ","
	 * Every comm should be a sequence of lower case characters (not enforced).
	 */
	op = from_user[0];
	if ((op != 'R' && op != 'D') || from_user[1] != ' ') {
		pr_err("DMT: hpl_proc_write: Illegal format or wrong operation: \"%s\"\n", from_user);
		return -EFAULT;
	}
	start = from_user + 2;
	end = strstr(start, ",");
	while (end) {
		*end = '\0';
		LAST_TIME:
		/* At this point, pointer start must be valid; however, end can be NULL */
		if (strlen(start) == 0) {  /* Case ",," or trailing "," */
			goto NEXT_COMM;
		}
		if (op == 'R') {
			/* Register */
			list_for_each(curr_head, &hpl_big_tea_list_head) {
				list = list_entry(curr_head, struct hpl_big_tea, head);
				if (!list->comm) {
					pr_err("DMT: hpl_proc_write: Invalid list entry\n");
					continue;
				}
				if (strcmp(list->comm, start) == 0) {
					pr_warn("DMT: hpl_proc_write: Comm %s was already registered to use big TEA\n", list->comm);
					goto NEXT_COMM;
				}
			}
			list = kmem_cache_alloc(hpl_big_tea_cache, GFP_KERNEL);
			if (!list) {
				pr_err("DMT: hpl_proc_write: Unable to alloc new list entry\n");
				return -EFAULT;
			}
			INIT_LIST_HEAD(&list->head);
			list->comm = kzalloc(strlen(start) + 1, GFP_KERNEL);
			if (!list->comm) {
				pr_err("DMT: hpl_proc_write: Unable to alloc comm string\n");
				kmem_cache_free(hpl_big_tea_cache, list);
				return -EFAULT;
			}
			strncpy(list->comm, start, strlen(start));
			list->comm[strlen(start)] = '\0';
			list_add_tail(&list->head, &hpl_big_tea_list_head);
			pr_warn("DMT: hpl_proc_write: Registered comm %s to use big TEA\n", list->comm);
		} else if (op == 'D') {
			/* Delete */
			list_for_each_safe(curr_head, tmp, &hpl_big_tea_list_head) {
				list = list_entry(curr_head, struct hpl_big_tea, head);
				if (!list->comm) {
					pr_err("DMT: hpl_proc_write: Invalid list entry\n");
				}
				if (strcmp(list->comm, start) == 0) {
					kfree(list->comm);
					list_del(curr_head);
                    kmem_cache_free(hpl_big_tea_cache, list);
					pr_warn("DMT: hpl_proc_write: Deleted comm %s\n", start);
					goto NEXT_COMM;
				}
			}
			pr_warn("DMT: hpl_proc_write: Comm %s not found\n", start);
		}
		NEXT_COMM:
		if (!end) {
			/* case LAST_TIME */
			kfree(from_user);
			return size;
		}
		start = end + 1;
		end = strstr(start, ",");
	}
	goto LAST_TIME;
}

static struct proc_ops hpl_proc_ops = {
        .proc_open    = hpl_proc_open,
        .proc_read    = seq_read,
        .proc_write   = hpl_proc_write,
        .proc_lseek   = seq_lseek,
        .proc_release = seq_release,
};

static bool hpl_proc_init(void) {
	hpl_big_tea_cache = kmem_cache_create("hpl_big_tea", sizeof(struct hpl_big_tea), 0, SLAB_PANIC, NULL);
	if (!hpl_big_tea_cache) {
		pr_err("DMT: hpl_proc_init: Big TEA cache is not ready, DMT is not usable\n");
		return false;
	} else {
		/* Proc filesystem init, run once */
		INIT_LIST_HEAD(&hpl_big_tea_list_head);

		hpl_proc_dir = proc_mkdir_mode(HPL_PROC_DIR, HPL_PROC_DIR_PERMISSION, NULL);
		if (hpl_proc_dir) {
			hpl_proc_entry = proc_create(HPL_PROC_ENTRY, HPL_PROC_ENTRY_PERMISSION, hpl_proc_dir, &hpl_proc_ops);
		}

		if (!hpl_proc_entry) {
			pr_err("DMT: hpl_proc_init: Unable to create proc fs entry\n");
		} else {
			pr_warn("DMT: hpl_proc_init: Created /proc/"HPL_PROC_DIR"/"HPL_PROC_ENTRY"\n");
		}
	}
	return true;
}

/**
 * Check if mm is configured and enabled with HPL.
*/
static bool hpl_mm_enabled(struct mm_struct *mm) {
	if (!mm) {
		return false;
	}

	if (mm->hpl_info.hpl_enabled == -1) {
		return false;
	} else if (mm->hpl_info.hpl_enabled == 1) {
		return true;
	}

	mm->hpl_info.hpl_enabled = 1;
	return true;
}

/**
 * Check if HPL should handle the operation
*/
static bool hpl_operation_check(struct mm_struct *mm, unsigned long vpmd, unsigned long vaddr, struct vm_area_struct *vma) {

	if (!hpl_seg_cachep) {
		return false;
	}
	
	if (vpmd <= 0) {
		return false;
	}

	if (!mm || !mm->owner) {
		return false;
	}

	if (mm->owner->pid <= 1) {
		return false;
	}

	if (system_state != SYSTEM_RUNNING) {
		return false;
	}

	if (!hpl_mm_enabled(mm)) {
		return false;
	}

	/* Init proc fs if necessary */
	if (!hpl_big_tea_cache && !hpl_proc_init()) {
		return false;
	}

	/* 4MB VMA Check */
	if (vma == NULL) {
		return false;
	}
	if ((vma->vm_end < vma->vm_start) || HPL_VADDR_RANGE_TO_SIZE(vma->vm_end, vma->vm_start) < HPL_VMA_THRESHOLD) {
		return false;
	}

	#if HPL_VMA_IGR_CHECK
	/* Extreme VMA Check */
	if (vma->vm_end == HPL_FIVE_LVL_VMA_IGR || vma->vm_end == HPL_FOUR_LVL_VMA_IGR) {
		return false;
	}
	#endif /* HPL_VMA_IGR_CHECK */

	return true;
}

/**
 *  TEA/Segment migrate
 *  PTE move code by Jiyuan
 */
 /* Depedency */
static bool hpl_seg_free(struct hpl_segment *seg, struct mm_struct *mm);

static int hpl_walk_move_pte(struct mm_struct *mm, struct vm_area_struct *vma, pmd_t *pmd, struct page *src, struct page *dst) {
	int ret = 0;
	ulong spfn = page_to_pfn(src);
	ulong dpfn = page_to_pfn(dst);
	pmd_t old = *pmd;
	spinlock_t *ptl;

	//pr_warn("DMT: hpl_walk_move_pte: Moving table page %lx to page %lx\n", spfn, dpfn);

	// Skipping empty entry, bad entry and leaf entry (i.e. huge pages)
	if (pmd_none(old) || pmd_bad(old) || pmd_leaf(old)) {
		pr_err("DMT: hpl_walk_move_pte: Failed to validate page table page %lx\n", spfn);
		return -ENOTSUPP;
	}

	//pr_warn("Moving page table page %lx to page %lx\n", spfn, dpfn);

	// Copy page table page content and bits
	ret = migrate_page(NULL, dst, src, MIGRATE_SYNC);
	if (ret) {
		pr_err("DMT: hpl_walk_move_pte: Failed to migrate page %lx to page %lx: %d\n", spfn, dpfn, ret);
		return -EAGAIN;
	}

	ptl = pmd_lock(mm, pmd);

	// Update entry value, seems no need to sync every bit?
	pmd_populate(mm, pmd, dst);

	spin_unlock(ptl);

	return 0;
}

static int hpl_walk_pmd(pmd_t *pmd, unsigned long addr, unsigned long next, struct mm_walk *walk) {
	struct mm_struct *mm = walk->mm;
	struct vm_area_struct *vma = walk->vma;
	struct page *pgtbl;
	struct page *target;
	unsigned long vpmd = HPL_VADDR_TO_VPMD(addr);

	if (!mm || !vma || !pmd || !hpl_walk_seg_migrating) {
		return 0;
	}

	/* Already moved? */
	if (hpl_walk_seg_new.hpl_bitmap[vpmd - hpl_walk_seg_new.start] == HPL_POOL_TAKEN) {
		return 0;
	}

	if (vpmd < hpl_walk_seg_new.start || vpmd >= hpl_walk_seg_new.end) {
		pr_err("DMT: hpl_walk_pmd: VPMD out of range, vaddr=%lx, vpmd=%lx, [%lx:%lx)\n", addr, vpmd, hpl_walk_seg_new.start, hpl_walk_seg_new.end);
		return 0;
	}
	pgtbl = pmd_page(*pmd);
	target = hpl_walk_seg_new.hpl_pool + (vpmd - hpl_walk_seg_new.start);

	if (!pgtbl || !target) {
		pr_err("DMT: hpl_walk_pmd: Failed to acquire page table page: src=%d, dst=%d\n", !!pgtbl, !!target);
		return 0;
	}

	/* Known issue -- does it really comes from DMT/HPL? */
	if (hpl_walk_seg_migrating->hpl_pool + (vpmd - hpl_walk_seg_migrating->start) != pgtbl) {
		//pr_err("DMT: hpl_walk_pmd: Got PTE outside the migrating pool, vpmd=%lx, bitmap=%d\n", vpmd, hpl_walk_seg_migrating->hpl_bitmap[vpmd - hpl_walk_seg_migrating->start]);
		return 0;
	}

	/* Move page table */
	//pr_warn("DMT: hpl_walk_pmd: Migrating HPL PTE Page: vaddr=%lx, vpmd=%lx, [%lx:%lx)\n", addr, vpmd, hpl_walk_seg_new.start, hpl_walk_seg_new.end);
	if (hpl_walk_move_pte(mm, vma, pmd, pgtbl, target) == 0) {
		hpl_walk_seg_new.hpl_bitmap[vpmd - hpl_walk_seg_new.start] = HPL_POOL_TAKEN;
		hpl_walk_seg_new.usage = hpl_walk_seg_new.usage + 1;
		//pr_warn("DMT: hpl_walk_pmd: Success!\n");
	}

	return 0;
}

static void hpl_walk_clean(struct mm_walk *walk)
{
	struct vm_area_struct *vma = walk->vma;

	if (!vma)
		return;

	// Modifying page table requires a flush
	//pr_warn("Flushed VMA range %lx to %lx\n", vma->vm_start, vma->vm_end);
	flush_tlb_range(vma, vma->vm_start, vma->vm_end);
}

static struct mm_walk_ops hpl_walk_ops = {
	.pmd_entry = hpl_walk_pmd,
	.post_vma = hpl_walk_clean,
};

static bool hpl_seg_migrate(struct hpl_segment *seg, struct mm_struct *mm, unsigned long ext_len, int mode) {
	unsigned long new_start, new_end, new_len, tm_start, tm_end, tm_ms, pmd_i;
	struct page *new_pool;
	bool *new_bitmap;
	int walk_ret = 0;

	new_start = seg->start;
	new_end = seg->end;
	new_len = seg->len + ext_len;

	if (mode == HPL_EXT_REAR) {
		new_end += ext_len;
	} else if (mode == HPL_EXT_FRONT) {
		new_start -= ext_len;
	} else {
		pr_err("DMT: hpl_seg_migrate: Illegal mode: %d\n", mode);
		return false;
	}

	/* Might sleep, unlock */
	spin_unlock(&mm->hpl_info.hpl_lock);

	/* Allocated migrated pool */
	tm_start = ktime_get_ns();
	new_bitmap = kvzalloc(new_len, GFP_KERNEL);
	new_pool = alloc_contig_pages(new_len, GFP_PGTABLE_USER, numa_node_id(), NULL);
	tm_end = ktime_get_ns();
	tm_ms = (tm_end - tm_start) / 1000000UL;
	if (HPL_TEST_PRINT_TIME || tm_ms) {
		if (!new_bitmap) {
			pr_err("DMT: hpl_seg_migrate: Got null bitmap, new_len=%lu\n", new_len);
		}
		if (!new_pool) {
			pr_err("DMT: hpl_seg_migrate: Got null pool, new_len=%lu\n", new_len);
		}
		pr_warn("DMT: hpl_seg_migrate: Migrate alloc, time=%lums, vpmd=[%lx:%lx), len=%lu, mm=%p\n", tm_ms, new_start, new_end, new_len, mm);
	}
	spin_lock(&mm->hpl_info.hpl_lock);
	if (!new_bitmap || !new_pool) {
		goto FAILED;
	}

	/* Charge local segment for page walk/copy access */
	hpl_walk_seg_new.start = new_start;
	hpl_walk_seg_new.end = new_end;
	hpl_walk_seg_new.len = new_len;
	hpl_walk_seg_new.usage = 0;
	hpl_walk_seg_new.hpl_pool = new_pool;
	hpl_walk_seg_new.hpl_bitmap = new_bitmap;
	hpl_walk_seg_migrating = seg;

	/* Walk page table */
	pr_warn("DMT: hpl_seg_migrate: Migrating: vaddr=[%lx:%lx), mode=%d, vpmd=[%lx:%lx)->[%lx:%lx), ext_len=%ld, len=%ld->%ld\n", HPL_VPMD_TO_VADDR(seg->start), HPL_VPMD_TO_VADDR(seg->end), mode, seg->start, seg->end, new_start, new_end, ext_len, seg->len, new_len);
	//mmap_write_lock(mm);
	//pr_warn("DMT: hpl_seg_migrate: Got lock\n");
	walk_ret = walk_page_range(mm, HPL_VPMD_TO_VADDR(seg->start), HPL_VPMD_TO_VADDR(seg->end), &hpl_walk_ops, NULL);
	//mmap_write_unlock(mm);
	if (walk_ret) {
		pr_err("DMT: hpl_seg_migrate: Walk failed, ret=%d\n", walk_ret);
		goto FAILED;
	}

	/* Double check bitmap */
	for (pmd_i = 0; pmd_i < seg->len; ++pmd_i) {
		if (mode == HPL_EXT_FRONT) {
			if (seg->hpl_bitmap[pmd_i] != hpl_walk_seg_new.hpl_bitmap[pmd_i + ext_len]) {
				//pr_err("DMT: hpl_seg_migrate: Bitmap mismatch, pmd_i=%lu, old=%d, new=%d\n", pmd_i, seg->hpl_bitmap[pmd_i], hpl_walk_seg_new.hpl_bitmap[pmd_i + ext_len]);
				hpl_walk_seg_new.hpl_bitmap[pmd_i + ext_len] = seg->hpl_bitmap[pmd_i];
			}
		} else {
			if (seg->hpl_bitmap[pmd_i] != hpl_walk_seg_new.hpl_bitmap[pmd_i]) {
				//pr_err("DMT: hpl_seg_migrate: Bitmap mismatch, pmd_i=%lu, old=%d, new=%d\n", pmd_i, seg->hpl_bitmap[pmd_i], hpl_walk_seg_new.hpl_bitmap[pmd_i]);
				hpl_walk_seg_new.hpl_bitmap[pmd_i] = seg->hpl_bitmap[pmd_i];
			}
		}
	}

	if (seg->usage != hpl_walk_seg_new.usage) {
		pr_err("DMT: hpl_seg_migrate: Usage mismatch, old=%lu, new=%lu\n", seg->usage, hpl_walk_seg_new.usage);
	}

	/* Release old segment */
	if (hpl_seg_free(seg, mm)) {
		*seg = hpl_walk_seg_new;
		pr_warn("DMT: hpl_seg_migrate: Success!!\n");
		return true;
	}

	FAILED:
	if (new_bitmap) {
		kvfree(new_bitmap);                                                                                                                                                                                                                                                                                                                                                                    
	}
	if (new_pool) {
		free_contig_range(page_to_pfn(new_pool), new_len);
	}
	return false;
}


/**
 * Given a segment
 * 
*/
static bool hpl_seg_append(struct hpl_segment *seg, struct mm_struct *mm, unsigned long ext_len, int mode) {
    unsigned long new_start, new_end, new_len, pfn_start, pfn_end;
    int result = -1;
    bool *new_bitmap;

	new_start = seg->start;
	new_end = seg->end;
	new_len = seg->len + ext_len;

	if (mode == HPL_EXT_REAR) {
		new_end += ext_len;
		pfn_start = page_to_pfn(seg->hpl_pool) + seg->len;
		pfn_end = page_to_pfn(seg->hpl_pool) + new_len;
	} else if (mode == HPL_EXT_FRONT) {
		new_start -= ext_len;
		pfn_start = page_to_pfn(seg->hpl_pool) - ext_len;
		pfn_end = page_to_pfn(seg->hpl_pool);
	} else {
		pr_err("DMT: hpl_seg_append: Illegal mode: %d\n", mode);
		return false;
	}

	/* Actual expanding: [start, end) */
	pr_warn("DMT: hpl_seg_append: mode=%d, vpmd=[%lx:%lx)->[%lx:%lx), ext_len=%ld, len=%ld->%ld, pfn:[%lx:%lx)->[%lx:%lx)\n", mode, seg->start, seg->end, new_start, new_end, ext_len, seg->len, new_len, page_to_pfn(seg->hpl_pool), page_to_pfn(seg->hpl_pool) + seg->len, pfn_start, pfn_end);
	if (new_start >= new_end || pfn_start >= pfn_end) {
		pr_err("DMT: hpl_seg_append: Illegal start and end\n");
		return false;
	}
	spin_unlock(&mm->hpl_info.hpl_lock);
	result = alloc_contig_range(pfn_start, pfn_end, MIGRATE_MOVABLE, GFP_PGTABLE_USER);
	new_bitmap = kvzalloc(new_len, GFP_KERNEL);
	spin_lock(&mm->hpl_info.hpl_lock);

	/* Check result */
	if (!new_bitmap) {
		pr_err("DMT: hpl_seg_append: Got null bitmap, new_len=%ld\n", new_len);
	}
	if (result < 0) {
		pr_err("DMT: hpl_seg_append: Got null pool, code=%d\n", result);
	}
	if (result < 0 || !new_bitmap) {
        if (new_bitmap) {
			kvfree(new_bitmap);
		}
		return false;
    }

	/* Bookkeeping */
	seg->start = new_start;
	seg->end = new_end;
	seg->len = new_len;
    memmove(new_bitmap, seg->hpl_bitmap, seg->len);
    kvfree(seg->hpl_bitmap);
    seg->hpl_bitmap = new_bitmap;

	pr_warn("DMT: hpl_seg_append: Success!\n");
    return true;
}

static bool hpl_seg_expand(struct hpl_segment *seg, struct mm_struct *mm, unsigned long ext_len, int mode) {
	#if HPL_ALLOW_EXPAND
	bool ret = hpl_seg_append(seg, mm, ext_len, mode);
	if (!ret) {
		ret = hpl_seg_migrate(seg, mm, ext_len, mode);
	}
	return ret;
	#else
	return false;
	#endif /* HPL_ALLOW_EXPAND */
}

/**
 * Check if a page in the pool is in the expected status based on the page number
*/
/* For testing purposes only */
#if HPL_TEST_EXPAND
#warning "Testing expand functionality"
static bool expand_test = true;
#endif /* HPL_TEST_EXPAND */
static bool hpl_page_check(struct hpl_segment *seg, struct mm_struct *mm, unsigned long vpmd, bool status, bool allow_expand) {
	#if HPL_TEST_EXPAND
	if (expand_test && allow_expand) {
		hpl_seg_expand(seg, mm, seg->len, HPL_EXT_REAR);
		hpl_seg_expand(seg, mm, seg->len, HPL_EXT_FRONT);
		expand_test = false;
	}
	#endif /* HPL_TEST_EXPAND */
	if (vpmd >= seg->end && allow_expand) {
		/* Try double the TEA size */
		hpl_seg_expand(seg, mm, seg->len, HPL_EXT_REAR);
	}

	if (vpmd < seg->start && allow_expand) {
		hpl_seg_expand(seg, mm, seg->len, HPL_EXT_FRONT);
	}

	if (vpmd < seg->start || vpmd >= seg->end) {
		pr_err("DMT: hpl_page_check: VPMD out of range: %lx, [%lx:%lx), len=%lu\n", vpmd, seg->start, seg->end, seg->len);
		return false;
	}

	if (!seg->hpl_bitmap || !seg->hpl_pool) {
		pr_err("DMT: hpl_page_check: Segment not charged: bitmap=%p, pool=%p\n", seg->hpl_bitmap, seg->hpl_pool);
		return false;
	}

	if (seg->hpl_bitmap[vpmd - seg->start] != status) {
		//pr_err("DMT: hpl_page_check: Unexpected status: expected=%d, actual=%d, vpmd=%lx, [%lx:%lx)\n", status, seg->hpl_bitmap[vpmd - seg->start], vpmd, seg->start, seg->end);
		return false;
	}

	return true;
}

/**
 * Find the page based on the page number, set it to the expected status
*/
static bool hpl_page_set(struct hpl_segment *seg, unsigned long vpmd, bool status) {
	if (!seg) {
		pr_err("DMT: hpl_page_set: Null segment given\n");
		return false;
	}
	
	if (vpmd < seg->start || vpmd >= seg->end) {
		pr_err("DMT: hpl_page_set: VPMD out of range: %lx, [%lx:%lx), len=%lu\n", vpmd, seg->start, seg->end, seg->len);
		return false;
	}

	if (!seg->hpl_bitmap || !seg->hpl_pool) {
		pr_err("DMT: hpl_page_set: Segment not charged: bitmap=%p, pool=%p\n", seg->hpl_bitmap, seg->hpl_pool);
		return false;
	}

	seg->hpl_bitmap[vpmd - seg->start] = status;

	return true;
}

/**
 * Find the page based on the page number, take it if possible
*/
static struct page *hpl_page_take(struct hpl_segment *seg, struct mm_struct *mm, unsigned long vpmd, bool allow_expand) {
	if (!hpl_page_check(seg, mm, vpmd, HPL_POOL_FREE, allow_expand)) {
		//pr_err("DMT: hpl_page_take: [FAILED WITH PAGE: %p]\n", seg->hpl_pool + (vpmd - seg->start));
		return NULL;
	}

	if (hpl_page_set(seg, vpmd, HPL_POOL_TAKEN)) {
		//pr_warn("returning\n");
		seg->usage = seg->usage + 1;  // ++ does not work here?
		return seg->hpl_pool + (vpmd - seg->start);
	}
    
	pr_err("DMT: hpl_page_take: Failed\n");
	return NULL;
}

/**
 * Find the page based on the page number, return it if possible
*/
static bool hpl_page_put(struct hpl_segment *seg, struct mm_struct *mm, unsigned long vpmd, struct page *page) {
	if (!hpl_page_check(seg, mm, vpmd, HPL_POOL_TAKEN, HPL_CHECK_NO_EXPAND)) {
		return false;
	}

	if (seg->hpl_pool + (vpmd - seg->start) != page) {
		/* Known issue */
		//pr_err("DMT: hpl_page_put: Page outside the pool\n");
		return false;
	}

	if (hpl_page_set(seg, vpmd, HPL_POOL_FREE)) {
		seg->usage = seg->usage - 1;
		return true;
	}

	pr_err("DMT: hpl_page_put: Failed\n");
	return false;
}

/*
	SAFE MARGIN
	This is for resolving the bug on CS 423 QEMU environment only.

	Run script cs423-q after init
	fork cs423-q, creates an mm
	After fork exec the new program image, the fork mm is released and a new mm is created
	After the new program exits, return to cs423-q, fork, and creates another mm
	After fork, the scheduler finds that the old program has stopped and releases the mm created after the exec
	Then based on the fork, exec the new program image, the fork mm is released and a new mm is created, usually directly using the last mm (just a coincidence).
	After the new program exits, return to cs423-q to continue forking
	And so on......

	IN THE STEP:
	-> Based on the fork, exec the new program image, the fork mm is released and a new mm is created, usually directly using the last mm
	--> After creating a new mm, a new HPL segment is created when the first physical page is requested
	---> Panic: Corrupted page table

	It remains unclear, why. Have read code for a long time.
	Here's a patch to minimalize the impact on the RAM: when freeing the segment and when we encounter a occupied page,
	Do not free it, and make a safe margin and do not free them (the following pages).
	Seems to be working fine.
*/
#define SAFE_MARGIN_SIZE 31
#if HPL_TEST_CS423_ENV
#warning "Building for CS 423 Environment"
static unsigned long safe_margin = 0;
#endif /* HPL_TEST_CS423_ENV */

/**
 * Free a segment, deallocate a pool and reseting its status
*/
static bool hpl_seg_free(struct hpl_segment *seg, struct mm_struct *mm) {
	unsigned long i;
	struct page *freeing;

	for (i = seg->start; i < seg->end; i++) {
		freeing = hpl_page_take(seg, mm, i, HPL_CHECK_NO_EXPAND);
		#if HPL_TEST_CS423_ENV
		if (freeing && !safe_margin) {
			__free_page(freeing);
		} else {
			if (!freeing) {
				safe_margin = SAFE_MARGIN_SIZE;
			} else {
				safe_margin--;
			}
		}
		#else
		if (freeing) {
			__free_page(freeing);
		}
		#endif /* HPL_TEST_CS423_ENV */
	}

	kvfree(seg->hpl_bitmap);

    seg->usage = 0;
	seg->hpl_bitmap = NULL;
	seg->hpl_pool = NULL;
	return true;
}

unsigned long ctr = 0;

/**
 * Create a segment, allocate a pool
*/
static bool hpl_seg_create(struct hpl_segment *seg, struct mm_struct *mm, unsigned long vpmd, unsigned long vaddr, struct vm_area_struct *vma, bool bigtea, char *comm, unsigned long tgid) {
	unsigned long vpmd_start, vpmd_end, vpmd_len, tm_start, tm_end, tm_ms;

	//pr_warn("DMT: hpl_seg_create: Creating segment...\n");
	vpmd_start = HPL_VADDR_TO_VPMD(vma->vm_start);
	vpmd_end = HPL_VADDR_TO_VPMD(vma->vm_end) + 1;  /* Round up... */
    vpmd_len = vpmd_end - vpmd_start;

	if (bigtea) {
		pr_warn("DMT: hpl_seg_create: Big TEA overide for comm %s(%lu), original vpmd=[%lx:%lx), len=%lu, HPL_BIG_TEA_LEN=%lu\n", comm, tgid, vpmd_start, vpmd_end, vpmd_len, HPL_BIG_TEA_LEN);
		vpmd_len = HPL_BIG_TEA_LEN * 2 + 1;
		vpmd_start = vpmd_start < HPL_BIG_TEA_LEN ? 0 : vpmd_start - HPL_BIG_TEA_LEN;
		vpmd_end = vpmd_start + vpmd_len;
	}

	/* FOR SAFETY: double check VMA range */
	if (vpmd_start >= vpmd_end) {
		pr_err("DMT: hpl_seg_create: Invalid VPMD start and end\n");
		return false;
	}

	/* FOR SAFETY: check vaddr within range */
	if (vaddr > HPL_VPMD_TO_VADDR(vpmd_end) || vaddr < HPL_VPMD_TO_VADDR(vpmd_start)) {
		pr_err("DMT: hpl_seg_create: Address outside the usable range: %lx, range=[%lx:%lx), vpmd=[%lx:%lx), len=%lu\n", vaddr,  HPL_VPMD_TO_VADDR(vpmd_start), HPL_VPMD_TO_VADDR(vpmd_end), vpmd_start, vpmd_end, vpmd_len);
		return false;
	}

	/* Might sleep, unlock */
	spin_unlock(&mm->hpl_info.hpl_lock);

	/* Charge the segment here */
	tm_start = ktime_get_ns();
	seg->hpl_bitmap = kvzalloc(vpmd_len, GFP_KERNEL);
	seg->hpl_pool = alloc_contig_pages(vpmd_len, GFP_PGTABLE_USER, numa_node_id(), NULL);
	tm_end = ktime_get_ns();
	ctr++;
	tm_ms = (tm_end - tm_start) / 1000000UL;
	if (HPL_TEST_PRINT_TIME || tm_ms) {
		if (!seg->hpl_bitmap) {
			pr_err("DMT: hpl_seg_create: Got null bitmap, vaddr_len=%luMB, vpmd_len=%lu\n", HPL_VADDR_RANGE_TO_SIZE(vma->vm_end, vma->vm_start), vpmd_len);
		}
		if (!seg->hpl_pool) {
			pr_err("DMT: hpl_seg_create: Got null pool, vaddr_len=%luMB, vpmd_len=%lu\n", HPL_VADDR_RANGE_TO_SIZE(vma->vm_end, vma->vm_start), vpmd_len);
		}
		pr_warn("DMT: hpl_seg_create: Alloc %lu, time=%lums, vma=%luMB, vpmd=[%lx:%lx), len=%lu, bigtea=%d, comm=%s(%ld), mm=%p\n", ctr, tm_ms, HPL_VADDR_RANGE_TO_SIZE(vma->vm_end, vma->vm_start), vpmd_start, vpmd_end, vpmd_len, bigtea, comm, tgid, mm);
	}
	spin_lock(&mm->hpl_info.hpl_lock);

	if (!seg->hpl_bitmap || !seg->hpl_pool) {
		if (seg->hpl_bitmap) {
			kvfree(seg->hpl_bitmap);
		}
		if (seg->hpl_pool) {
			free_contig_range(page_to_pfn(seg->hpl_pool), vpmd_len);
		}
        seg->hpl_bitmap = NULL;
        seg->hpl_pool = NULL;
        return false;
    }

	seg->start = vpmd_start;
	seg->end = vpmd_end;
    seg->len = vpmd_len;
    seg->usage = 0;
    
	//pr_warn("DMT: Segment created\n");
	return true;
}

/**
 * Given a segment and a page number that is outside the pool, check if we can merge.
 * Merge if possible. Update and allocate a new pool.
*/
static bool __maybe_unused hpl_seg_merge(struct hpl_segment *seg, struct mm_struct *mm, unsigned long vpmd, unsigned long vaddr, struct vm_area_struct *vma) {
    unsigned long vpmd_start, vpmd_end, vpmd_len, new_start, new_end, new_len, ext_len;
    int occupation;

    vpmd_start = HPL_VADDR_TO_VPMD(vma->vm_start);
	vpmd_end = HPL_VADDR_TO_VPMD(vma->vm_end) + 1;
    vpmd_len = vpmd_end - vpmd_start;
	if (vpmd_start >= vpmd_end) {
		pr_err("DMT: hpl_seg_merge: Segment merge failed because of invalid VPMD start and end\n");
		return false;
	}

    /* Check by case: VMA positions */
	/**
	*  [NEW VMA] [OLD VMA] => [MERGED VMA]
	*  New start & end: vpmd_start, seg->end
	*  New size: seg->end - vpmd_start
	*
	**/
	if (vpmd_end < seg->start) {
        new_start = vpmd_start;
        new_end = seg->end;
        new_len = seg->end - vpmd_start;
		ext_len = seg->start - vpmd_start; /* seg->end - vpmd_start - seg->end + seg->start */
		occupation = HPL_MERGE_OCCUPATION(new_len, vpmd_len, seg->len);
		pr_warn("DMT: hpl_seg_merge: Front merge, occupation=%d, expected start=%lx, end=%lx, len=%ld\n", occupation, new_start, new_end, new_len);
        if (occupation >= HPL_MERGE_THRESHOLD) {
			return hpl_seg_expand(seg, mm, new_len, HPL_EXT_FRONT);
        }
        return false;
    }

    /**
	*  [OLD VMA] [NEW VMA] => [MERGED VMA]
	*  New start & end: seg->start, vpmd_end
	*  New size: vpmd_end - seg->start
	*
	**/
	if (vpmd_start >= seg->end) {
        new_start = seg->start;
        new_end = vpmd_end;
        new_len = vpmd_end - seg->start;
		ext_len = vpmd_end - seg->end; /* vpmd_end - seg->start - seg->end + seg->start */
		occupation = HPL_MERGE_OCCUPATION(new_len, vpmd_len, seg->len);
		pr_warn("DMT: hpl_seg_merge: Rear merge, occupation=%d, expected start=%lx, end=%lx, len=%ld\n", occupation, new_start, new_end, new_len);
        if (occupation >= HPL_MERGE_THRESHOLD) {
			return hpl_seg_expand(seg, mm, new_len, HPL_EXT_REAR);
		}
		return false;
    }

    return false;
}

/**
 * Based on the page number, find a segment.
 * If segment is not present, then try to create a segment based on the VMA.
 * Also, try to merge the segment.
*/
static int hpl_seg_seek(struct mm_struct *mm, unsigned long vpmd, unsigned long vaddr, struct vm_area_struct *vma, bool chargable, char *comm, unsigned long tgid) {
	struct hpl_segment *new_seg;
	int seg_i;
	bool bigtea = false;
	struct list_head *curr_head = NULL;
	struct hpl_big_tea *list = NULL;

	/* Try to find segment */
	if (vma->hpl_seg != 0 && vma->hpl_seg <= HPL_SEG_LIMIT && mm->hpl_info.segments[vma->hpl_seg - 1] && mm->hpl_info.occupied[vma->hpl_seg - 1]) {
		return vma->hpl_seg;
	}

	#if HPL_SEG_INCLUSION_CHECK
	/* Allocated? */
	unsigned long vpmd_start, vpmd_end;
	vpmd_start = HPL_VADDR_TO_VPMD(vma->vm_start);
	vpmd_end = HPL_VADDR_TO_VPMD(vma->vm_end) + 1;
	for (seg_i = 0; seg_i < HPL_SEG_LIMIT; ++seg_i) {
		if (mm->hpl_info.segments[seg_i] && mm->hpl_info.occupied[seg_i]) {
			if (vpmd_start >= mm->hpl_info.segments[seg_i]->start && vpmd_end <= mm->hpl_info.segments[seg_i]->end) {
				pr_warn("DMT: hpl_seg_seek: VMA vpmd [%lx:%lx) is contained within segment %d vpmd [%lx:%lx)\n", vpmd_start, vpmd_end, seg_i, mm->hpl_info.segments[seg_i]->start, mm->hpl_info.segments[seg_i]->end);
				vma->hpl_seg = seg_i + 1;
				return vma->hpl_seg;
			}
		}
	}
	#endif /* HPL_SEG_INCLUSION_CHECK */

	/* Prepare to create new segment */
	if (!chargable) {
		//pr_err("DMT: hpl_seg_seek: Segment is not chargable\n");
		return 0;
	}
	if (mm->hpl_info.occupied_count >= HPL_SEG_LIMIT) {
		pr_err("DMT: hpl_seg_seek: Ran out of slot\n");
		return 0;
	}
	new_seg = NULL;

	/* Big TEA Override */
	list_for_each(curr_head, &hpl_big_tea_list_head) {
		list = list_entry(curr_head, struct hpl_big_tea, head);
		if (strcmp(list->comm, comm) == 0) {
			bigtea = true;
			break;
		}
	}

	/* Find next available slot; also check merge */
    for (seg_i = 0; seg_i < HPL_SEG_LIMIT; ++seg_i) {
		if (mm->hpl_info.occupied[seg_i] == true) {
			/* Big TEA ignore merge */
			if (bigtea) {
				continue;
			}
            /* Merge check */
			#if HPL_MERGE_CHECK
            if (hpl_seg_merge(mm->hpl_info.segments[seg_i], mm, vpmd, vaddr, vma)) {
				vma->hpl_seg = seg_i + 1;
                return vma->hpl_seg;
            }
			#endif /* HPL_MERGE_CHECK */
            continue;
        }
		new_seg = kmem_cache_alloc(hpl_seg_cachep, GFP_KERNEL);
		break;
	}
	if (!new_seg) {
		pr_err("DMT: hpl_seg_seek: Unable to allocate segment, occupied_count=%d\n", mm->hpl_info.occupied_count);
		return 0;
	}
	if (!hpl_seg_create(new_seg, mm, vpmd, vaddr, vma, bigtea, comm, tgid)) {
		kmem_cache_free(hpl_seg_cachep, new_seg);
		//pr_err("DMT: hpl_seg_seek: Unable to create segment\n");
		return 0;
	}
	vma->hpl_seg = seg_i + 1;
	mm->hpl_info.segments[seg_i] = new_seg;
	mm->hpl_info.occupied[seg_i] = true;
	mm->hpl_info.occupied_count = mm->hpl_info.occupied_count + 1;
	//pr_warn("DMT: hpl_seg_seek: seek return, address=%p, new count: %d\n", new_seg, mm->hpl_info.occupied_count);

	return vma->hpl_seg;
}

/**
 * Allocate a page using Hyperlane.
*/
static struct page *hpl_page_alloc_impl(struct mm_struct *mm, gfp_t gfp, unsigned long vpmd, unsigned long vaddr, struct vm_area_struct *vma) {
	struct page *page = NULL;
	struct hpl_segment *seg = NULL;
	int seg_i = 0;
	char *comm = "UNKNOWN";
	unsigned long tgid = 0;

	comm = mm->owner->comm;
	tgid = mm->owner->tgid;
	
	spin_lock(&mm->hpl_info.hpl_lock);
	//pr_err("DMT: hpl_page_alloc_impl: Try alloc\n");

	seg_i = hpl_seg_seek(mm, vpmd, vaddr, vma, true, comm, tgid);
	if (!seg_i) {
		//pr_err("DMT: hpl_page_alloc_impl: Segment not found or not able to create\n");
		goto FAILED;
    }
	seg = HPL_SEG_I_TO_SEG(mm, seg_i);
	page = hpl_page_take(seg, mm, vpmd, HPL_CHECK_ALLOW_EXPAND);
	if (!page){
		pr_err("DMT: hpl_page_alloc_impl: Unable to take page out\n");
		goto FAILED;
    }

	spin_unlock(&mm->hpl_info.hpl_lock);
	//pr_warn("DMT: give page\n");
	return page;

FAILED:
	spin_unlock(&mm->hpl_info.hpl_lock);
	pr_warn("DMT: hpl_page_alloc_impl: Failed to alloc hpl page for comm %s (%lu), vpmd=%lx, seg=%d, page=%d\n", comm, tgid, vpmd, !!seg, !!page);
	return alloc_page(gfp);
}

/**
 * Free a page using Hyperlane.
*/
static void hpl_page_free_impl(struct mm_struct *mm, struct page *page, unsigned long vpmd, unsigned long vaddr, struct vm_area_struct *vma) {
    bool success;
	int seg_i;
	struct hpl_segment *seg;
	char *comm;
	unsigned long tgid;

	comm = mm->owner->comm;
	tgid = mm->owner->tgid;

	spin_lock(&mm->hpl_info.hpl_lock);

	seg_i = hpl_seg_seek(mm, vpmd, vaddr, vma, false, comm, tgid);
	if (!seg_i) {
		//pr_err("DMT: hpl_page_free_impl: Segment not found\n");
		goto FAILED;
    }
	seg = HPL_SEG_I_TO_SEG(mm, seg_i);

	success = hpl_page_put(seg, mm, vpmd, page);
	if (!success) {
		pr_err("DMT: hpl_page_free_impl: Unable to take page out\n");
		goto FAILED;
    }
    
    /* Is the segment still being used? */
    if (seg->usage == 0) {
    	if (hpl_seg_free(seg, mm)) {
			pr_warn("DMT: hpl_page_free_impl: Recycled\n");
			kmem_cache_free(hpl_seg_cachep, seg);
			mm->hpl_info.occupied_count = mm->hpl_info.occupied_count - 1;
			mm->hpl_info.occupied[seg_i - 1] = false;
		} else {
			pr_warn("DMT: hpl_page_free_impl: Unable to recycle\n");
		}
    }

	//pr_warn("DMT: hpl_page_free_impl: Usage left: %lu\n", seg->usage);

	spin_unlock(&mm->hpl_info.hpl_lock);
	return;

FAILED:
	spin_unlock(&mm->hpl_info.hpl_lock);
	pr_warn("DMT: hpl_page_free_impl: Failed to put back page %lx for comm %s (%lu)\n", page_to_pfn(page), comm, tgid);
	//pr_warn("DMT: hpl_page_free_impl: HPL page free failed\n");
	__free_page(page);
	return;
}


/* DMT HC Dependencies */
#define TEA_MODE(vmid, is_free, nested_lvl) (0ULL | (((uint64_t)(vmid)) << 32) | ((is_free) ? 0x100 : 0) | ((nested_lvl) & 0xff))
extern u64 kvm_hc_alloc_tea(u64 gva, u64 gpa, u64 pgcnt, u64 mode);

static __maybe_unused bool hpl_hypercall(unsigned long mode) {
	unsigned long hc_ret, pfn_start, pfn_end, pfn_range;
	int reserve_ret;

	if (mode != HPL_HC_MODE_ALLOC && mode != HPL_HC_MODE_FREE) {
		pr_err("DMT: hpl_hypercall: Illegal mode: %lu\n", mode);
		return false;
	}

	if (mode == HPL_HC_MODE_FREE) {
		pfn_start = (hpl_hc_gpa - PAGE_SIZE * HPL_HC_PGCNT) >> PAGE_SHIFT;
		pfn_end = hpl_hc_gpa >> PAGE_SHIFT;
		pfn_range = pfn_end - pfn_start;
		pr_warn("DMT: hpl_hypercall: Freeing mapped memory from host, page_size=%lu, page_shift=%d, pfn=[%lu:%lu), range=%lu\n", PAGE_SIZE, PAGE_SHIFT, pfn_start, pfn_end, pfn_range);
		//free_contig_range(pfn_start, pfn_range);
	}

	pr_warn("DMT: hpl_hypercall: Making hypercall: gva=%lu, gpa=%lu, pgcnt=%lu(sys), %lu(dmt), mode=%lu, page_size=%lu, page_shift=%d\n", hpl_hc_gva, hpl_hc_gpa, max_pfn, HPL_HC_PGCNT, mode, PAGE_SIZE, PAGE_SHIFT);
	hc_ret = kvm_hc_alloc_tea(hpl_hc_gva, hpl_hc_gpa, HPL_HC_PGCNT, mode);
	if (hc_ret) {
		pr_err("DMT: hpl_hypercall: Hypercall failed, ret=%lu", hc_ret);
		return false;
	}

	if (mode == HPL_HC_MODE_FREE) {
		hpl_hc_gva--;
		hpl_hc_gpa = hpl_hc_gpa - PAGE_SIZE * HPL_HC_PGCNT;
		pr_warn("DMT: hpl_hypercall: Finished successfully\n");
	}
	
	if (mode == HPL_HC_MODE_ALLOC) {
		pfn_start = hpl_hc_gpa >> PAGE_SHIFT;
		hpl_hc_gva++;
		hpl_hc_gpa = hpl_hc_gpa + PAGE_SIZE * HPL_HC_PGCNT;
		pfn_end = hpl_hc_gpa >> PAGE_SHIFT;
		pfn_range = pfn_end - pfn_start;
		pr_warn("DMT: hpl_hypercall: Reserving mapped memory from host, page_size=%lu, page_shift=%d, pfn=[%lu:%lu), range=%lu\n", PAGE_SIZE, PAGE_SHIFT, pfn_start, pfn_end, pfn_range);
		//reserve_ret = alloc_contig_range(pfn_start, pfn_end, MIGRATE_MOVABLE, GFP_PGTABLE_USER);
		if (reserve_ret < 0) {
			hc_ret = kvm_hc_alloc_tea(hpl_hc_gva, hpl_hc_gpa, HPL_HC_PGCNT, HPL_HC_MODE_FREE);
			pr_err("DMT: hpl_hypercall: Memory reserve failed, reserve_ret=%d, unmap_ret=%lu\n", reserve_ret, hc_ret);
			return false;
		}
		pr_warn("DMT: hpl_hypercall: Finished successfully\n");
	}

	return true;
}

/* Public Functions */
/**
 * Run as soon as the system is ready [Public]
*/
void __init hpl_global_init(void) {
	/* Slab cache */
    hpl_seg_cachep = kmem_cache_create("hpl_segment", sizeof(struct hpl_segment), 0, SLAB_PANIC, NULL);

	if (!hpl_seg_cachep) {
		pr_err("DMT: hpl_global_init: Segment cache is not ready\n");
	}

	// /* Hypercall */
	// if (hpl_hypercall(HPL_HC_MODE_ALLOC)) {
	// 	pr_warn("DMT: hpl_global_init: Welcome to DMT/Hyperlane\n");
	// }

    return;
}

/**
 * Initialize the hyperlane data sctructure in mm [Public]
*/
void hpl_mm_init(struct mm_struct *mm) {
	int seg_i;

    spin_lock_init(&mm->hpl_info.hpl_lock);
    mm->hpl_info.hpl_enabled = 0;

	for (seg_i = 0; seg_i < HPL_SEG_LIMIT; ++seg_i) {
		mm->hpl_info.segments[seg_i] = NULL;
		mm->hpl_info.occupied[seg_i] = false;
	}

	mm->hpl_info.occupied_count = 0;

	//pr_warn("DMT: hpl_mm_init: New MM HPL info block: %p\n", mm);
}

/**
 * Free all segments in mm_struct [Public]
*/
void hpl_mm_free(struct mm_struct *mm) {
    struct hpl_struct *info;
    int seg_i;

	info = &mm->hpl_info;

	if (!info) {
		pr_warn("DMT: hpl_mm_free: Invalid hpl_info\n");
		return;
	}

	//pr_warn("DMT: hpl_mm_free: Freeing MM HPL segments on mm=%p, count:%d\n", mm, info->occupied_count);

	spin_lock(&info->hpl_lock);

	for (seg_i = 0; seg_i < HPL_SEG_LIMIT; ++seg_i) {
		if (info->occupied[seg_i] == false || !info->segments[seg_i]) {
			continue;
		}
		//pr_err("DMT: hpl_mm_free: Freeing segment %d (%lu) on mm %p, new count: %d\n", seg_i, info->segments[seg_i]->len, mm, info->occupied_count);

		// if (info->segments[seg_i]->usage > 0) {
		// 	pr_err("DMT: hpl_mm_free: Segment %d is in use!!\n", seg_i);
		// } else 
		if (hpl_seg_free(info->segments[seg_i], mm)){
			kmem_cache_free(hpl_seg_cachep, info->segments[seg_i]);
			info->occupied[seg_i] = false;
			info->segments[seg_i] = NULL;
			info->occupied_count = info->occupied_count - 1;			
		}
	}
	//dump_stack();

	info->hpl_enabled = 0;

	spin_unlock(&info->hpl_lock);

	//pr_warn("DMT: hpl_mm_free: MM freed\n");
}

void hpl_vma_free(struct mm_struct *mm, struct vm_area_struct *vma) {

}

/**
 * Allocate a page using Hyperlane [Public]
*/
struct page *hpl_page_alloc(struct mm_struct *mm, gfp_t gfp, unsigned long addr) {
	unsigned long vpmd = HPL_VADDR_TO_VPMD(addr);
	struct page *page = NULL;
	struct vm_area_struct *vma = vma_lookup(mm, addr);
	
	if (!hpl_operation_check(mm, vpmd, addr, vma)) {
		page = alloc_page(gfp);
	} else {
		page = hpl_page_alloc_impl(mm, gfp, vpmd, addr, vma);
		//pr_err("[PAGE GIVE OUT BY DMT: %p]\n", page);
	}

	return page;
}

/**
 * Free a page using Hyperlane [Public]
*/
void hpl_page_free(struct mm_struct *mm, struct page *page, unsigned long addr) {
	unsigned long vpmd = HPL_VADDR_TO_VPMD(addr);
	struct vm_area_struct *vma = vma_lookup(mm, addr);

	if (!page) {
		return;
	}

	if (!hpl_operation_check(mm, vpmd, addr, vma)) {
		//pr_err("[FREEING ERR PAGE: %p]\n", page);
		__free_page(page);
	} else {
		hpl_page_free_impl(mm, page, vpmd, addr, vma);
	}
}
