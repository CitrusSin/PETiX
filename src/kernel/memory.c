#include <petix/petix.h>
#include <petix/memory.h>

#define KERNEL_STACK_SIZE 2048

void memory_init(int magic, void *ards_ptr) {
    u32 count;
    ards_t *ards;

    if (magic == PETIX_MAGIC) {
        printk("PETiX ARDS address:\t%p\n", ards_ptr);
        count = *(u32 *)ards_ptr;
        ards = (ards_t *)((u32*)ards_ptr + 1);
    }

    for (size_t i=0; i<count; i++) {
        printk("ARDS#%u: \tbase=%p,\tsize=%u,\ttype=%u\n", i, (u32)ards[i].base, (u32)ards[i].size, ards[i].type);
    }
}

void *ptxmem_kmalloc(size_t required_size) {
    
}