#ifndef PETIX_ALGORITHM_H
#define PETIX_ALGORITHM_H

#include "./types.h"

void* insert_pq_heap(
    void* begin,
    void* end,
    size_t element_size,
    bool(*comp)(void*, void*)
);

void* pop_pq_heap(
    void* begin,
    void* end,
    size_t element_size,
    bool(*comp)(void*, void*)
);

void make_pq_heap(
    void* begin,
    void* end,
    size_t element_size,
    bool(*comp)(void*, void*)
);

#endif