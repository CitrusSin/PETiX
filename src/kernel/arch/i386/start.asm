[bits 32]

extern kernel_init

global _start
_start:
    call kernel_init
.lp0:
    hlt
    jmp .lp0