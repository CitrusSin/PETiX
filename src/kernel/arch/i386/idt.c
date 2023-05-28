#include <petix/petix.h>
#include <petix/assert.h>
#include <petix/arch/i386.h>

static gate_descriptor_t idt[IDT_SIZE];
static ptr_dt idt_ptr;

extern void* interrupt_entry_table[IDT_SIZE];
interrupt_handler_t interrupt_handler_table[IDT_SIZE];

static void register_interrupt(int number, void* ptr) {
    idt[number].offset_0_15 = ((u32)ptr) & 0xffff;
    idt[number].offset_16_31 = ((u32)ptr >> 16) & 0xffff;
}

void register_interrupt_handler(int number, interrupt_handler_t handler) {
    interrupt_handler_table[number] = handler;
}

static void interrupt_exception_handler(int number, cpu_i386_interrupt_snapshot_t* regs) {
    const char* msg = "Unknown exception";
    switch (number) {
    case 0:
        msg = "Divided by zero";
        break;
    }

    char register_status[256];
    sprintf(
        register_status,
        "EAX=%#010x, EBX=%#010x, ECX=%#010x, EDX=%#010x\nESI=%#010X, EDI=%#010x, EBP=%#010x, ESP=%#010x\n CS=    %#06x, EIP=%#010x, EFR=%#010x",
        regs->eax, regs->ebx, regs->ecx, regs->edx,
        regs->esi, regs->edi, regs->ebp, regs->esp,
        regs->cs, regs->eip, regs->eflags
    );

    char segment_register_status[256];
    sprintf(
        segment_register_status,
        "DS=%#06x, ES=%#06x, FS=%#06x, GS=%#06x\nSS=%#06x",
        regs->ds, regs->es, regs->fs, regs->gs, regs->ss
    );

    panic(
        "Exceptional interrupt %#02x: %s\nRegisters:\n%s\nSegment registers:\n%s\n",
        number, msg, register_status, segment_register_status
    );
}

void idt_init() {
    for (size_t i=0; i<IDT_SIZE; i++) {
        gate_descriptor_t *gt = &idt[i];
        gt->offset_0_15 = NULL;
        gt->offset_16_31 = NULL;
        gt->DPL = 0;
        gt->present = true;
        gt->reserved = 0;
        gt->segment = false;
        gt->selector = kernel_code_segment();
        gt->type = 0b1110;
    }

    for (size_t i=0; i<IDT_SIZE; i++) {
        register_interrupt(i, interrupt_entry_table[i]);
    }

    for (size_t i=0; i<0x20; i++) {
        register_interrupt_handler(i, interrupt_exception_handler);
    }

    idt_ptr.addr = idt;
    idt_ptr.size = sizeof(idt)-1;
    asm volatile("lidt idt_ptr");
    PTDEBUG("Interrupt set up.");
}