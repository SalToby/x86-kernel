#include <stdint.h>
#include "pic.h"
#include "print.h"
volatile uint64_t ticks = 0;


void interrupt_handler(uint32_t vector) {
    uint32_t irq = vector - 32;
    
    switch (irq) {
        case 0: // PIT timer
            ticks++;
            break;
        // case 1: keyboard ...
        // case 12: mouse ...
        default:
            break; // unhandled IRQ
    }

    PIC_sendEOI(irq);
}