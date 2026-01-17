#pragma once
#include <stdint.h>
#include <stdbool.h>

#define IDT_MAX_DESCRIPTORS 256

typedef struct __attribute__((packed)) {
    uint16_t isr_low;
    uint16_t kernel_cs;
    uint8_t  reserved;
    uint8_t  attributes;
    uint16_t isr_high;
} idt_entry_t;

typedef struct __attribute__((packed)) {
    uint16_t limit;
    uint32_t base;
} idtr_t;

void idt_init(void);
void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags);


