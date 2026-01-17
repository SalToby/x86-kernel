#include "pit.h"

static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile ( "outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
    /* There's an outb %al, $imm8 encoding, for compile-time constant port numbers that fit in 8b. (N constraint).
     * Wider immediate constants would be truncated at assemble-time (e.g. "i" constraint).
     * The  outb  %al, %dx  encoding is the only option for all other cases.
     * %1 expands to %dx because  port  is a uint16_t.  %w1 could be used if we had the port number a wider C type */
}

void pit_init() {
    int divisor = 1193180 / 100;  // PIT base clock / desired frequency

    outb(0x43, 0x36);  // Command byte: channel 0, mode 3, low+high byte
    outb(0x40, divisor & 0xFF);      // low byte
    outb(0x40, (divisor >> 8) & 0xFF); // high byte
}
