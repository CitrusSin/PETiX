#ifndef PETIX_IO_H
#define PETIX_IO_H

#include "./types.h"

#define OUT8(port, value) __asm__ __volatile__("out %%al, %%dx" : : "a"((u8)(value)), "d"((u16)(port)))
#define IN8(var, port) __asm__ __volatile__("in %%dx, %%al" : "=a"(var) : "d"((u16)(port)))
#define OUT16(port, value) __asm__ __volatile__("out %%ax, %%dx" : : "a"((u16)(value)), "d"((u16)(port)))
#define IN16(var, port) __asm__ __volatile__("in %%dx, %%ax" : "=a"(var) : "d"((u16)(port)))

#endif