#ifndef _HYPERLANE_TYPES_H
#define _HYPERLANE_TYPES_H

#define HPL_SEG_LIMIT 16

struct hpl_segment {
    unsigned long start;
    unsigned long end;
    unsigned long len;
    unsigned long usage;
    struct page *hpl_pool;
    bool *hpl_bitmap;
};

struct hpl_struct {
    spinlock_t hpl_lock;
    struct hpl_segment *segments[HPL_SEG_LIMIT];
    bool occupied[HPL_SEG_LIMIT];
    int occupied_count;
    int hpl_enabled;
};

#endif /* _HYPERLANE_TYPES_H */
