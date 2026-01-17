#include "idt.h"
#include "pic.h"
#include "pit.h"
#include "print.h"
#include "memory_boot_info.h"
#include "mmngr_phys.h"

void kernel_main(uint32_t magic, multiboot_info_t *mbi)
{
    idt_init();
    pic_init();
    pit_init();
    pmmngr_init((mbi->mem_lower+mbi->mem_upper));
    
    kprint("Kernel booted!\n");

    if (mbi->flags & (1 << 6)) {
        parse_boot_map(mbi->mmap_addr, mbi->mmap_length);
        
    } else {
        kprint("No memory map provided\n");
    }
    for (;;)
    {

        asm volatile("hlt");
    }
}