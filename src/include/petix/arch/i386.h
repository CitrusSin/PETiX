#ifndef PETIX_ARCHI386_GDT_H
#define PETIX_ARCHI386_GDT_H

#include "../types.h"

typedef struct segment_descriptor_t {
    u16 memory_limit_0_15;
    u16 memory_base_0_15;
    u8  memory_base_16_23;
    bool access_bit : 1;   // Left 0 for CPU
    bool read_write : 1;   // R for code, W for data
    bool direction_conforming : 1;  // C for code, D for data
    bool executable : 1;
    bool segment : 1;
    u8 DPL : 2;
    bool present : 1;
    u8 memory_limit_16_19 : 4;
    bool reserved : 1;
    bool long_mode : 1;     // 64 bit ext mark
    bool big_segment: 1;    // 32 or 16
    bool granularity : 1;
    u8 memory_base_24_31;
} __packed segment_descriptor_t;

typedef u16 segment_selector_t;

typedef struct gate_descriptor_t {
    u16 offset_0_15;
    segment_selector_t selector;
    u8 reserved;
    u8 type : 4;    // Interrupt: 0b1110 Trap: 0b1111
    bool segment : 1;
    u8 DPL : 2;
    bool present : 1;
    u16 offset_16_31;
} __packed gate_descriptor_t;

typedef struct ptr_dt {
    u16 size;
    void* addr;
} __packed ptr_dt;

typedef struct cpu_i386_interrupt_snapshot_t {
    u32 ds;
    u32 es;
    u32 fs;
    u32 gs;
    const u32 ss;
    u32 esi;
    u32 edi;
    u32 ebx;
    u32 ecx;
    u32 edx;
    const u32 esp;
    u32 ebp;
    u32 eax;
    u32 eip;
    u32 cs;
    u32 eflags;
} cpu_i386_interrupt_snapshot_t;

typedef void(*interrupt_handler_t)(int, cpu_i386_interrupt_snapshot_t*);

void gdt_init();
void idt_init();

void register_interrupt_handler(int number, interrupt_handler_t handler);

segment_selector_t kernel_code_segment();

#define GDT_SIZE 32
#define IDT_SIZE 256

#endif