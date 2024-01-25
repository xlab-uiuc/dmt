#ifndef _HYPERLANE_H
#define _HYPERLANE_H

#include <linux/types.h>
#include <linux/mm_types.h>
#include <linux/hyperlane_types.h>

void __init hpl_global_init(void);

void hpl_mm_init(struct mm_struct *mm);

void hpl_mm_free(struct mm_struct *mm);

void hpl_vma_free(struct mm_struct *mm, struct vm_area_struct *vma);

struct page *hpl_page_alloc(struct mm_struct *mm, gfp_t gfp, unsigned long addr);

void hpl_page_free(struct mm_struct *mm, struct page *page, unsigned long addr);

#endif /* _HYPERLANE_H */
