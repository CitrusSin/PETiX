[bits 32]

section .text

extern panic
extern interrupt_handler_table

%macro INTERRUPT_HANDLER_DECLARE 1
interrupt_%1:
    push %1
    jmp interrupt_entry
%endmacro

interrupt_entry:
    xchg eax, [esp] ; Store eax to stack and get the interrupt number

    push ebp
    mov ebp, esp

    push esp
    add dword [esp], (3*4)+(2*4)
    push edx
    push ecx
    push ebx
    push edi
    push esi
    push ss
    push gs
    push fs
    push es
    push ds

    ; EAX is the interrupt number
    push esp            ; Call with the register status struct
    push eax
    call [interrupt_handler_table + eax * 4]

    pop ds
    pop es
    pop fs
    pop gs
    add esp, 4 ; Do NOT modify ss
    pop esi
    pop edi
    pop ebx
    pop ecx
    pop edx
    add esp, 4  ; Do NOT modify esp

    mov esp, ebp
    pop ebp
    pop eax
    iret

INTERRUPT_HANDLER_DECLARE 0x00
INTERRUPT_HANDLER_DECLARE 0x01
INTERRUPT_HANDLER_DECLARE 0x02
INTERRUPT_HANDLER_DECLARE 0x03
INTERRUPT_HANDLER_DECLARE 0x04
INTERRUPT_HANDLER_DECLARE 0x05
INTERRUPT_HANDLER_DECLARE 0x06
INTERRUPT_HANDLER_DECLARE 0x07
INTERRUPT_HANDLER_DECLARE 0x08
INTERRUPT_HANDLER_DECLARE 0x09
INTERRUPT_HANDLER_DECLARE 0x0a
INTERRUPT_HANDLER_DECLARE 0x0b
INTERRUPT_HANDLER_DECLARE 0x0c
INTERRUPT_HANDLER_DECLARE 0x0d
INTERRUPT_HANDLER_DECLARE 0x0e
INTERRUPT_HANDLER_DECLARE 0x0f
INTERRUPT_HANDLER_DECLARE 0x10
INTERRUPT_HANDLER_DECLARE 0x11
INTERRUPT_HANDLER_DECLARE 0x12
INTERRUPT_HANDLER_DECLARE 0x13
INTERRUPT_HANDLER_DECLARE 0x14
INTERRUPT_HANDLER_DECLARE 0x15
INTERRUPT_HANDLER_DECLARE 0x16
INTERRUPT_HANDLER_DECLARE 0x17
INTERRUPT_HANDLER_DECLARE 0x18
INTERRUPT_HANDLER_DECLARE 0x19
INTERRUPT_HANDLER_DECLARE 0x1a
INTERRUPT_HANDLER_DECLARE 0x1b
INTERRUPT_HANDLER_DECLARE 0x1c
INTERRUPT_HANDLER_DECLARE 0x1d
INTERRUPT_HANDLER_DECLARE 0x1e
INTERRUPT_HANDLER_DECLARE 0x1f

section .data

global interrupt_entry_table
interrupt_entry_table:
dd interrupt_0x00
dd interrupt_0x01
dd interrupt_0x02
dd interrupt_0x03
dd interrupt_0x04
dd interrupt_0x05
dd interrupt_0x06
dd interrupt_0x07
dd interrupt_0x08
dd interrupt_0x09
dd interrupt_0x0a
dd interrupt_0x0b
dd interrupt_0x0c
dd interrupt_0x0d
dd interrupt_0x0e
dd interrupt_0x0f
dd interrupt_0x10
dd interrupt_0x11
dd interrupt_0x12
dd interrupt_0x13
dd interrupt_0x14
dd interrupt_0x15
dd interrupt_0x16
dd interrupt_0x17
dd interrupt_0x18
dd interrupt_0x19
dd interrupt_0x1a
dd interrupt_0x1b
dd interrupt_0x1c
dd interrupt_0x1d
dd interrupt_0x1e
dd interrupt_0x1f
times 256-(($-interrupt_entry_table)/4) dd 0

interrupt_response db "Divide by 0 ERROR", 10, \
    "eax=%#010x", 9, "ebx=%#010x", 9, "ecx=%#010x", 9, "edx=%#010x", 10, 0