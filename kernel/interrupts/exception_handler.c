#include <stdint.h>

__attribute__((noreturn))
void exception_handler(uint32_t);
void exception_handler(uint32_t interrupt) {
    __asm__ volatile ("cli;"); // stop interrupts

    volatile char *video = (volatile char*)0xB8000;
    char arr[16] = "exception: ";
    
    // convert number to ASCII
    if (interrupt < 10) {
        arr[11] = '0' + interrupt;
        arr[12] = 0; // null terminate
    } else {
        arr[11] = '0' + interrupt / 10; // tens
        arr[12] = '0' + interrupt % 10; // ones
        arr[13] = 0; // null terminate
    }

    int colour = 0xF;
    char *string = arr;
    while (*string != 0) {
        *video++ = *string++;
        *video++ = colour;
    }
    

    __asm__ volatile ("hlt;");
}

