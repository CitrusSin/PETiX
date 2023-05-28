#ifndef PETIX_MEMORY_H
#define PETIX_MEMORY_H

#include "./types.h"

extern void *memmove(void* dst, void* src, size_t n);
#define memcpy memmove
void *memset(void* dst, unsigned char value,  size_t n);

#endif