#pragma once
#include <stdint.h>

// VGA text buffer
static volatile uint16_t* VGA_BUFFER = (volatile uint16_t*)0xB8000;
static uint16_t VGA_WIDTH = 80;
static uint16_t VGA_HEIGHT = 25;
static uint16_t cursor_x = 0;
static uint16_t cursor_y = 0;
static uint8_t  color = 0x0F; // White on black

// Helper: put a character on the screen
static inline void kputchar(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
        if (cursor_y >= VGA_HEIGHT) cursor_y = 0; // simple wrap
        return;
    }

    VGA_BUFFER[cursor_y * VGA_WIDTH + cursor_x] = (color << 8) | c;
    cursor_x++;
    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
        if (cursor_y >= VGA_HEIGHT) cursor_y = 0;
    }
}

static inline void kprint(const char* str) {
    while (*str) {
        kputchar(*str++);
    }
}

static inline void kprint_int(int64_t num) {
    char buf[12]; // enough for -2^31
    int i = 0;
    int is_negative = 0;

    if (num == 0) {
        kputchar('0');
        return;
    }

    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    while (num > 0) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }

    if (is_negative) {
        buf[i++] = '-';
    }

    // Print in reverse
    for (int j = i - 1; j >= 0; j--) {
        kputchar(buf[j]);
    }
}
