[bits 32]

section .text

extern panic
extern interrupt_handler_table

%macro INTERRUPT_HANDLER_DECLARE 2
interrupt_%1:
%ifn %2
    push 0x00000000
%endif
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
    push dword [esp+14*4]
    push eax
    call [interrupt_handler_table + eax * 4]
    add esp, 3*4

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
    add esp, 4
    iret

INTERRUPT_HANDLER_DECLARE 0x00, 0
INTERRUPT_HANDLER_DECLARE 0x01, 0
INTERRUPT_HANDLER_DECLARE 0x02, 0
INTERRUPT_HANDLER_DECLARE 0x03, 0

; INTERRUPT_HANDLER_DECLARE 0x04, 0
; Do not handle overflow exception
interrupt_0x04:
    iret

INTERRUPT_HANDLER_DECLARE 0x05, 0
INTERRUPT_HANDLER_DECLARE 0x06, 0
INTERRUPT_HANDLER_DECLARE 0x07, 0
INTERRUPT_HANDLER_DECLARE 0x08, 1
INTERRUPT_HANDLER_DECLARE 0x09, 0
INTERRUPT_HANDLER_DECLARE 0x0a, 1
INTERRUPT_HANDLER_DECLARE 0x0b, 1
INTERRUPT_HANDLER_DECLARE 0x0c, 1
INTERRUPT_HANDLER_DECLARE 0x0d, 1
INTERRUPT_HANDLER_DECLARE 0x0e, 1
INTERRUPT_HANDLER_DECLARE 0x0f, 0
INTERRUPT_HANDLER_DECLARE 0x10, 0
INTERRUPT_HANDLER_DECLARE 0x11, 0
INTERRUPT_HANDLER_DECLARE 0x12, 0
INTERRUPT_HANDLER_DECLARE 0x13, 0
INTERRUPT_HANDLER_DECLARE 0x14, 0
INTERRUPT_HANDLER_DECLARE 0x15, 0
INTERRUPT_HANDLER_DECLARE 0x16, 0
INTERRUPT_HANDLER_DECLARE 0x17, 0
INTERRUPT_HANDLER_DECLARE 0x18, 0
INTERRUPT_HANDLER_DECLARE 0x19, 0
INTERRUPT_HANDLER_DECLARE 0x1a, 0
INTERRUPT_HANDLER_DECLARE 0x1b, 0
INTERRUPT_HANDLER_DECLARE 0x1c, 0
INTERRUPT_HANDLER_DECLARE 0x1d, 0
INTERRUPT_HANDLER_DECLARE 0x1e, 0
INTERRUPT_HANDLER_DECLARE 0x1f, 0
INTERRUPT_HANDLER_DECLARE 0x20, 0
INTERRUPT_HANDLER_DECLARE 0x21, 0
INTERRUPT_HANDLER_DECLARE 0x22, 0
INTERRUPT_HANDLER_DECLARE 0x23, 0
INTERRUPT_HANDLER_DECLARE 0x24, 0
INTERRUPT_HANDLER_DECLARE 0x25, 0
INTERRUPT_HANDLER_DECLARE 0x26, 0
INTERRUPT_HANDLER_DECLARE 0x27, 0
INTERRUPT_HANDLER_DECLARE 0x28, 0
INTERRUPT_HANDLER_DECLARE 0x29, 0
INTERRUPT_HANDLER_DECLARE 0x2a, 0
INTERRUPT_HANDLER_DECLARE 0x2b, 0
INTERRUPT_HANDLER_DECLARE 0x2c, 0
INTERRUPT_HANDLER_DECLARE 0x2d, 0
INTERRUPT_HANDLER_DECLARE 0x2e, 0
INTERRUPT_HANDLER_DECLARE 0x2f, 0

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
dd interrupt_0x20
dd interrupt_0x21
dd interrupt_0x22
dd interrupt_0x23
dd interrupt_0x24
dd interrupt_0x25
dd interrupt_0x26
dd interrupt_0x27
dd interrupt_0x28
dd interrupt_0x29
dd interrupt_0x2a
dd interrupt_0x2b
dd interrupt_0x2c
dd interrupt_0x2d
dd interrupt_0x2e
dd interrupt_0x2f
times 256-(($-interrupt_entry_table)/4) dd 0

interrupt_response db "Divide by 0 ERROR", 10, \
    "eax=%#010x", 9, "ebx=%#010x", 9, "ecx=%#010x", 9, "edx=%#010x", 10, 0