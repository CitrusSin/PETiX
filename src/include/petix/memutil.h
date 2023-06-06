#ifndef PETIX_MEMUTIL_H
#define PETIX_MEMUTIL_H

#include "./types.h"

extern void *memmove(void* dst, void* src, size_t n);
#define memcpy memmove
void *memset(void* dst, unsigned char value, size_t n);
void memxchg(void* p1, void* p2, size_t n);

#endif