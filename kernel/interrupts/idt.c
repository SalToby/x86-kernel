#include "idt.h"

__attribute__((aligned(0x10)))
static idt_entry_t idt[IDT_MAX_DESCRIPTORS];

static idtr_t idtr;
static bool vectors[IDT_MAX_DESCRIPTORS];

extern void* isr_stub_table[];
extern void* irq_stub_table[];

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags) {
    idt_entry_t* descriptor = &idt[vector];

    descriptor->isr_low   = (uint32_t)isr & 0xFFFF;
    descriptor->kernel_cs = 0x08;
    descriptor->attributes = flags;
    descriptor->isr_high  = (uint32_t)isr >> 16;
    descriptor->reserved  = 0;
}

void idt_init(void) {
    idtr.base = (uintptr_t)&idt[0];
    idtr.limit = sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

    //exceptions
    for (uint8_t vector = 0; vector < 32; vector++) {
        idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
        vectors[vector] = true;
    }

    //interrupts
    for (uint8_t vector = 0; vector < 16; vector++) {
        idt_set_descriptor(vector+32, irq_stub_table[vector], 0x8E);
        vectors[vector] = true;
    }

    __asm__ volatile ("lidt %0" : : "m"(idtr));
    __asm__ volatile ("sti");
}
