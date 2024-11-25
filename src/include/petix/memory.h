#ifndef PETIX_MEMORY_H
#define PETIX_MEMORY_H

#include <petix/types.h>

typedef struct ards_t {
    u64 base;
    u64 size;
    u32 type;
} __packed ards_t;

void memory_init(int magic, void *ards_ptr);

void *ptxmem_kmalloc(size_t required_size);

#endif