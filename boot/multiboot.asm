; ==============================
; Multiboot header (NASM)
; ==============================

BITS 32

%define ALIGN    (1 << 0)
%define MEMINFO  (1 << 1)
%define FLAGS    (ALIGN | MEMINFO)
%define MAGIC    0x1BADB002
%define CHECKSUM -(MAGIC + FLAGS)

section .multiboot
align 4
dd MAGIC
dd FLAGS
dd CHECKSUM

; ==============================
; Stack
; ==============================

section .bss
align 16
stack_bottom:
    resb 16384        ; 16 KiB
stack_top:


section .data
align 8
gdt_start:
    dq 0x0000000000000000    ; Null
    dq 0x00CF9A000000FFFF    ; Code
    dq 0x00CF92000000FFFF    ; Data
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start


; ==============================
; Entry point
; ==============================

section .text
global _start
extern kernel_main

_start:

    lgdt [gdt_descriptor]
    mov ax, 0x10       ; data segment selector (index 2 << 3)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, stack_top
    ; Far jump to reload CS
    jmp 0x08:flush_cs   ; 0x08 = code segment in your GDT

    flush_cs:
        push ebx        
        push eax 
        call kernel_main

    cli
.hang:
    hlt
    jmp .hang
