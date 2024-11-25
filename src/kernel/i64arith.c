#include <petix/types.h>

#define ABS(x) ((x<0)?(-(x)):(x))

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

i64 __divdi3(i64 a, i64 b) {
    bool s1 = a<0, s2 = b<0;
    bool s = s1 ^ s2;
    i64 val = __udivdi3(ABS(a), ABS(b));
    if (s) val = -val;
    return val;
}

i64 __moddi3(i64 a, i64 b) {
    bool s1 = a<0;
    i64 val = __umoddi3(ABS(a), ABS(b));
    if (s1) val = -val;
    return val;
}