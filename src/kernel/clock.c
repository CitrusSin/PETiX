#include <petix/petix.h>
#include <petix/io.h>
#include <petix/assert.h>

#define CLOCK_CTRL 0x43
#define CLOCK_0 0x40
#define CLOCK_1 0x41
#define CLOCK_2 0x42

#define BPR_CTRL 0x61

#define OSCILLATOR_HZ 1193182

static u64 ms_counter = 0;

void delay(u32 ms) {
    ENTER_NON_INTERRUPT;

    u64 target = ms_counter + ms;
    while (ms_counter < target) {
        asm volatile(
            "sti\n"
            "hlt\n"
            "cli"
        );
    }
    
    EXIT_NON_INTERRUPT;
}

u64 uptime_ms() {
    ENTER_NON_INTERRUPT;
    u64 osc = ms_counter;
    EXIT_NON_INTERRUPT;
    return osc;
}

void bpr_set_enable(bool enable) {
    u8 ctrl_word;
    IN8(ctrl_word, BPR_CTRL);
    if (enable)
        ctrl_word |= 0b00000011;
    else
        ctrl_word &= 0b11111100;
    OUT8(BPR_CTRL, ctrl_word);
}

void bpr_set_tune(int freq_hz) {
    int clock_count = OSCILLATOR_HZ / freq_hz;

    OUT8(CLOCK_CTRL, 0b10110110);
    OUT8(CLOCK_2, clock_count & 0xff);
    OUT8(CLOCK_2, (clock_count >> 8) & 0xff);
}

void beep(int freq_hz, int millisec) {
    bpr_set_tune(freq_hz);
    bpr_set_enable(true);
    delay(millisec);
    bpr_set_enable(false);
}

static void clock_interrupt_handler(int number, int error_code, cpu_i386_interrupt_snapshot_t *regs) {
    assert(number == CLOCK_INTERRUPT);
    ms_counter++;
    pic_send_eoi(number);
}

void clock_init() {
    const int clock_count = 1193;

    register_interrupt_handler(CLOCK_INTERRUPT, clock_interrupt_handler);
    OUT8(CLOCK_CTRL, 0b00110100);
    OUT8(CLOCK_0, clock_count&0xff);
    OUT8(CLOCK_0, (clock_count>>8)&0xff);
    PTDEBUG("Clock init done.");
}