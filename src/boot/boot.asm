[org 0x7c00]

mov ax, 3
int 0x10

xor ax, ax
mov ds, ax
mov es, ax
mov ss, ax
mov sp, 0x7c00

mov bx, msg
call print

mov di, 0x1000
mov ecx, 2
mov bl, 4
call rdisk

cmp word [0x1000], 0xaa55
jne error

jmp 0:0x1002

error:
    mov bx, errormsg
    call print
    .hltl:
        hlt
        jmp .hltl

; ds:[di] Destination, ecx: Begi@ext:doinkythederp.nasm-language-supportn sector, bl: Sector count
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
        push cx
        mov cx, 256 ; Sector size
        cld
        mov ax, ds
        push es
        mov es, ax
        .rdlp:
            in ax, dx
            nop
            stosw
            loop .rdlp
        pop es
        pop cx
        loop .read
    ret

; print content of ds:[bx]
print:
    push ax
    push si
    mov ah, 0x0e
    xor si, si
.next:
    mov al, [bx+si]
    cmp al, 0
    jz .done
    int 0x10
    inc si
    jmp .next
.done:
    pop si
    pop ax
    ret

msg db "Petix Bootloader v0.1", 13, 10, 0
errormsg db "ERROR: Failed to load loader. Halting.", 13, 10, 0
times 510-($-$$) db 0

db 0x55, 0xaa