#include <petix/petix.h>
#include <petix/types.h>
#include <petix/memutil.h>

static segment_descriptor_t gdt[GDT_SIZE];
static ptr_dt gdt_desc;

void gdt_init() {
    PTDEBUG("Saving GDT.");

    memset(gdt, 0, sizeof(gdt));

    asm volatile("sgdt gdt_desc");
    memcpy(gdt, gdt_desc.addr, gdt_desc.size+1);
    gdt_desc.addr = gdt;
    gdt_desc.size = sizeof(gdt)-1;
    asm volatile("lgdt gdt_desc");

    PTDEBUG("GDT Save complete.");
}