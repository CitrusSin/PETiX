#include <petix/algorithm.h>
#include <petix/memutil.h>
#include <petix/types.h>

// Comp: less than
// Small top
void* insert_pq_heap(
    void* begin,
    void* end,
    size_t element_size,
    bool(*comp)(void*, void*)
) {
    char *cb = (char *)begin, *ce = (char *)end;
    size_t n = (ce - cb) / element_size;
    size_t r = n;
    while (r > 0) {
        size_t upper = (r+1)/2-1;
        char *upper_ptr = cb + upper * element_size;
        char *present_ptr = cb + r * element_size;
        if (comp(present_ptr, upper_ptr)) {
            memxchg(upper_ptr, present_ptr, element_size);
        } else break;
        r = upper;
    }
    return ce+element_size;
}

void* pop_pq_heap(
    void* begin,
    void* end,
    size_t element_size,
    bool(*comp)(void*, void*)
) {
    char *cb = (char *)begin, *ce = (char *)end;
    size_t n = (ce - cb) / element_size - 1;
    size_t r = 0;
    memxchg(cb, cb+n*element_size, element_size);
    while (r < n) {
        size_t chl = (r+1)*2-1;
        size_t chr = chl+1;
        if (chl > n) break;
        bool has_ch2 = chr < n;

        bool cmp1 = comp(cb+r, cb+chl);
        bool cmp2 = has_ch2 ? comp(cb+r, cb+chr) : true;
        if (cmp1 && cmp2) {
            break;
        }
        bool cmp3 = has_ch2 ? comp(cb+chl, cb+chr) : true;
        if (cmp3) {
            memxchg(cb+r, cb+chl, element_size);
            r = chl;
        } else {
            memxchg(cb+r, cb+chr, element_size);
            r = chr;
        }
    }
    return ce-element_size;
}

void make_pq_heap(
    void* begin,
    void* end,
    size_t element_size,
    bool(*comp)(void*, void*)
) {
    char *cb = (char *)begin, *ce = (char *)end;
    size_t n = (ce - cb) / element_size;
    for (size_t i=2; i<n; i++) {
        insert_pq_heap(cb, cb+(i-1)*element_size, element_size, comp);
    }
}