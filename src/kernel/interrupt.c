#include <petix/petix.h>
#include <petix/assert.h>
#include <petix/io.h>

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

static void interrupt_exception_handler(int number, int error_code, cpu_i386_interrupt_snapshot_t* regs) {
    static const char* interrupt_message_table[] = {
        "Dividing error",
        "Debug trap",
        "Unknown exception",
        "INT 0x03 trap",
        "Overflow",
        "Bound check failed",
        "Illegal opcode",
        "Device unavailable",
        "Double exception",
        "FPU segment out of bound",
        "Invalid TSS",
        "Segment not exist",
        "Stack fault",
        "Privilege protected",
        "Page fault",
        "Unknown exception",
        "FPU error"
    };
    const char* msg = "Unknown exception";
    if (number < 17) msg = interrupt_message_table[number];

    char register_status[256];
    sprintf(
        register_status,
        "EAX=%#010x, EBX=%#010x, ECX=%#010x, EDX=%#010x\nESI=%#010X, EDI=%#010x, EBP=%#010x, ESP=%#010x\nEIP=%#010x, EFLAGS=%#010x",
        regs->eax, regs->ebx, regs->ecx, regs->edx,
        regs->esi, regs->edi, regs->ebp, regs->esp,
        regs->eip, regs->eflags
    );

    char segment_register_status[256];
    sprintf(
        segment_register_status,
        "DS=%#06x, ES=%#06x, FS=%#06x, GS=%#06x\nCS=%#06x, SS=%#06x",
        regs->ds, regs->es, regs->fs, regs->gs, regs->cs, regs->ss
    );

    panic(
        "Exceptional interrupt %#04x: %s\nRegisters:\n%s\nSegment registers:\n%s\n",
        number, msg, register_status, segment_register_status
    );
}

static void idt_init() {
    for (size_t i=0; i<IDT_SIZE; i++) {
        gate_descriptor_t *gt = &idt[i];
        gt->offset_0_15 = NULL;
        gt->offset_16_31 = NULL;
        gt->DPL = 0;
        gt->present = true;
        gt->reserved = 0;
        gt->segment = false;
        gt->selector = KRNL_CODE_SEG;
        gt->type = 0b1110;
    }

    for (size_t i=0; i<IDT_SIZE; i++) {
        register_interrupt(i, interrupt_entry_table[i]);
    }

    for (size_t i=0; i<17; i++) {
        register_interrupt_handler(i, interrupt_exception_handler);
    }

    idt_ptr.addr = idt;
    idt_ptr.size = sizeof(idt)-1;
    asm volatile("lidt idt_ptr");
    PTDEBUG("Interrupt set up.");
}

#define PIC_M_CTRL 0x20
#define PIC_M_DATA 0x21
#define PIC_S_CTRL 0xa0
#define PIC_S_DATA 0xa1

#define PIC_CMD_EOI 0x20

void pic_send_eoi(int number) {
    if ((number & 0x0f) >= 8)
        OUT8(PIC_S_CTRL, PIC_CMD_EOI);
    OUT8(PIC_M_CTRL, PIC_CMD_EOI);
}

static void outer_interrupt_handler(int number, int error_code, cpu_i386_interrupt_snapshot_t* regs) {
    switch (number) {
    case 0x20:
        break;
    case 0x21:
        break;
    case 0x22:
        break;
    }
    pic_send_eoi(number);
}

void interrupt_init() {
    const u8 icw1 = 0b00010001;
    OUT8(PIC_M_CTRL, icw1);
    OUT8(PIC_M_DATA, PIC_BEGIN_INTERRUPT);
    OUT8(PIC_M_DATA, 0b00000100);
    OUT8(PIC_M_DATA, 0b00000001);

    OUT8(PIC_S_CTRL, icw1);
    OUT8(PIC_S_DATA, PIC_BEGIN_INTERRUPT + 0x08);
    OUT8(PIC_S_DATA, 2);
    OUT8(PIC_S_DATA, 0b00000001);

    OUT8(PIC_M_DATA, 0b11111110);
    OUT8(PIC_S_DATA, 0b11111111);


    idt_init();
    for (int num = PIC_BEGIN_INTERRUPT; num < PIC_BEGIN_INTERRUPT + 0x10; num++) {
        register_interrupt_handler(num, outer_interrupt_handler);
    }
}