#include <petix/petix.h>
#include <petix/console.h>
#include <petix/assert.h>
#include <petix/stdarg.h>

void assert_failure(const char* exp, const char* file, const char* base, int line) {
    panic("Assert %s failed:\n\t-- at %s, line %d.\n", exp, file, line);
}

void panic(const char* fmt, ...) {
    printk("\nSYSTEM PANIC: \n");

    va_list args;
    va_start(args, fmt);
    vprintk(fmt, args);
    va_end(args);

    printk("\nHALTING.\n");

    for (;;) asm volatile("hlt");
    asm volatile("ud2");
}