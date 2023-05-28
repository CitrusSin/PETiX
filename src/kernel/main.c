#include <petix/petix.h>
#include <petix/console.h>
#include <petix/string.h>
#include <petix/stdarg.h>
#include <petix/assert.h>
#include <petix/arch/i386.h>

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

void kernel_init()
{
    petix_console_init();
    petix_console_clear();

    gdt_init();
    idt_init();

    printk("PETiX v0.1\n");
    printk("Entering kernel\n");
    char cpu_type[49];
    printk("CPU Manufacturer: \t%s\n", get_cpu_manufacturer(cpu_type));
    printk("CPU Type: \t\t%s\n", get_cpu_type(cpu_type));

    int r = 8/0;
    printk("Test 3\n");
}
