#include <petix/types.h>

u64 __udivdi3(u64 a, u64 b) {
    if (b > 0xffffffff) return 0;
    union {
        u64 val;
        u16 bk[4];
    } p;
    p.val = a;
    u32 a4[5] = {(p.bk[3])&0xffff, (p.bk[2])&0xffff, (p.bk[1])&0xffff, (p.bk[0])&0xffff, 0};
    u32 b2 = b;
    u64 result = 0;
    for (int i=0; i<4; i++) {
        result <<= 16;
        result += a4[i]/b2;
        a4[i+1] += (a4[i]%b2) << 16;
    }
    return result;
}

u64 __umoddi3(u64 a, u64 b) {
    if (b > 0xffffffff) return 0;
    union {
        u64 val;
        u16 bk[4];
    } p;
    p.val = a;
    u32 a4[5] = {(p.bk[3])&0xffff, (p.bk[2])&0xffff, (p.bk[1])&0xffff, (p.bk[0])&0xffff, 0};
    u32 b2 = b;
    u64 result = 0;
    for (int i=0; i<4; i++) {
        result <<= 16;
        result += a4[i]/b2;
        a4[i+1] += (a4[i]%b2) << 16;
    }
    return a4[4] >> 16;
}