[org 0x1000]

dw 0xaa55

xor ax, ax
mov ds, ax
mov es, ax
mov ss, ax
mov sp, 0x1000

mov bx, text0
call print
; Detect memory
memory_detect:
    xor ebx, ebx
    mov di, ards_buf
    .loop:
        mov eax, 0xe820
        mov ecx, 20
        mov edx, 0x534d4150
        int 0x15
        jc error
        add di, 20
        inc dword [ards_count]
        cmp ebx, 0
        jnz .loop

prepare_protected_mode:
    ; Disable interrupt
    cli
    ; Open A20 address line
    in al, 0x92
    or al, 0b10
    out 0x92, al
    ; Load GDT and enter P mode
    lgdt [gdt_ptr]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    ; Refresh CS register cache and enter P mode
    jmp dword code_selector:protected_mode

error:
    mov bx, errormsg
    call print
    ; Stop running
    .hltl:
        hlt
        jmp .hltl

; Print content of ds:[bx]
print:
    push ax
    push si
    mov ah, 0x0e
    xor si, si
.lp:
    mov al, [bx+si]
    cmp al, 0
    jz .next
    int 0x10
    inc si
    jmp .lp
.next:
    pop si
    pop ax
    ret

text0 db "Booting system...", 13, 10, 0
errormsg db "System booting failed!", 13, 10, 0

[bits 32]
protected_mode:
    mov ax, data_selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ; Setup stack top
    mov esp, 0x10000
    ; Test
    ; mov byte [0xb8000], 'P'
    mov edi, 0x10000
    mov ecx, 10
    mov bl, 200
    call rdisk

    mov eax, 1346720841
    mov ebx, ards_count

    jmp dword code_selector:0x10000
    ud2

; [edi] Destination, ecx: Begin sector, bl: Sector count
rdisk:
    mov dx, 0x1f2
    mov al, bl
    out dx, al

    .lp:
        inc dx  ;0x1f3,0x1f4,0x1f5
        mov al, cl
        shr ecx, 8
        out dx, al
        cmp dx, 0x1f5
        jb .lp

    inc dx  ;0x1f6 Device control
    and cl, 0b1111
    mov al, 0b11100000  ; LBA Main disk
    or al, cl
    out dx, al

    inc dx
    mov al, 0x20    ; 0x20->0x1f7: Read disk
    out dx, al

    xor ecx, ecx
    mov cl, bl  ; Sector count

    .read:
        ; Wait for data
        mov dx, 0x1f7
        .chk:
            in al, dx
            nop
            and al, 0b10001000
            cmp al, 0b00001000
            jnz .chk
        
        ; Read data
        mov dx, 0x1f0
        push ecx
        mov ecx, 256 ; Sector size
        cld
        .rdlp:
            in ax, dx
            nop
            stosw
            loop .rdlp
        pop ecx
        loop .read
    ret


code_selector equ 1<<3
data_selector equ 2<<3
; Base physical address
mem_base equ 0
; mem_limit 4G/4K - 1 = 0xfffff
mem_limit equ (1024 * 1024 * 1024 * 4)/(1024 * 4) - 1

gdt_ptr:
    dw (gdt_end - gdt_base) - 1
    dd gdt_base
gdt_base:
    dd 0, 0
gdt_code:
    dw mem_limit & 0xffff
    dw mem_base & 0xffff
    db (mem_base >> 16) & 0xff
    ; Present_DPL_Segment_Type(X_C/E_R/W_A)
    db 0b1_00_1_1010
    ; Granularity + Big + Long mode + Available + Limit high
    db ((mem_limit>>16)&0xf) | 0b0_1_0_0_0000
    ; Base high
    db (mem_base >> 24)&0xff
gdt_data:
    dw mem_limit & 0xffff
    dw mem_base & 0xffff
    db (mem_base >> 16) & 0xff
    ; Present_DPL_Segment_Type(X_C/E_R/W_A)
    db 0b1_00_1_0010
    ; Granularity + Big + Long mode + Available + Limit high
    db ((mem_limit>>16)&0xf) | 0b0_1_0_0_0000
    ; Base high
    db (mem_base >> 24)&0xff
gdt_end:

ards_count dd 0
ards_buf:
