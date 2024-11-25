#include <petix/petix.h>
#include <petix/console.h>
#include <petix/string.h>
#include <petix/stdarg.h>
#include <petix/assert.h>
#include <petix/memory.h>
#include <petix/time.h>

char *get_cpu_manufacturer(char *str)
{
    u32 type = 0;
    u32 a, b, c, d;
    __asm__(
        "cpuid"
        : "=a"(a), "=b"(b), "=c"(c), "=d"(d)
        : "a"(type));
    u32 arr[4] = {b, d, c, 0};
    return strcpy(str, (const char *)arr);
}

char *get_cpu_type(char *str)
{
    u32 type = 0x80000000;
    u32 a, b, c, d;
    __asm__(
        "cpuid"
        : "=a"(a), "=b"(b), "=c"(c), "=d"(d)
        : "a"(type));
    if (a < 0x80000004)
    {
        return strcpy(str, "CPUID Type name unsupported");
    }
    u32 arr[13];
    int k = 0;
    for (type = 0x80000002; type <= 0x80000004; type++)
    {
        __asm__(
            "cpuid"
            : "=a"(a), "=b"(b), "=c"(c), "=d"(d)
            : "a"(type));
        arr[k++] = a;
        arr[k++] = b;
        arr[k++] = c;
        arr[k++] = d;
    }
    arr[12] = 0;
    return strcpy(str, (const char *)arr);
}

void kernel_init(int magic, void* ards_ptr)
{
    ptxcon_init();
    ptxcon_console_clear();

    printk("PETiX v0.1\n");
    printk("Entering kernel\n");
    
    memory_init(magic, ards_ptr);
    gdt_init();
    interrupt_init();
    clock_init();
    time_init();

    asm volatile (
        "sti"
    );

    char cpu_type[49];
    printk("CPU Manufacturer: \t%s\n", get_cpu_manufacturer(cpu_type));
    printk("CPU Type: \t\t%s\n", get_cpu_type(cpu_type));
    printk("\a");

    for (int i=0; i<1000; i++) {
        time_t tmt = time(NULL);
        char timetype[256];
        struct tm tmstr;
        gmtime_s(&tmstr, &tmt);
        strftime(timetype, 255, "%Y-%m-%d %H:%M:%S", &tmstr);
        printk("%s\r", timetype);
        delay(10);
    }

    int u = 1 / 0;
}
