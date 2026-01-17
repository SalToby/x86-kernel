%macro irq_stub 1
irq_stub_%+%1:
    pushad

    push ds
    push es
    push fs
    push gs

    mov ax, 0x10        ; kernel data selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push dword %1
    call interrupt_handler
    add esp, 4

    pop gs
    pop fs
    pop es
    pop ds

    popad
    iret
%endmacro

extern interrupt_handler

; IRQ0–15 mapped to IDT vectors 32–47
irq_stub 32
irq_stub 33
irq_stub 34
irq_stub 35
irq_stub 36
irq_stub 37
irq_stub 38
irq_stub 39
irq_stub 40
irq_stub 41
irq_stub 42
irq_stub 43
irq_stub 44
irq_stub 45
irq_stub 46
irq_stub 47

global irq_stub_table
irq_stub_table:
%assign i 32
%rep 16
    dd irq_stub_%+i
%assign i i+1
%endrep
