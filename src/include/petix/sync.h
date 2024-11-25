#ifndef PETIX_SYNC_H
#define PETIX_SYNC_H

#include "./types.h"

struct __ptx_semaphore_struct {
    int32_t value;
} ptx_semaphore_struct_t, *ptx_semaphore;

#endif
