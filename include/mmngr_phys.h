#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "string.h"

typedef uintptr_t physical_addr;

//symbol for end of kernel in linker
extern uintptr_t _kernel_end;

#define PAGE_ALIGN_UP(x) (((x) + PMMNGR_BLOCK_SIZE - 1) & ~(PMMNGR_BLOCK_SIZE - 1))
#define PAGE_ALIGN_DOWN(x) ((x) & ~(PMMNGR_BLOCK_SIZE  - 1))

typedef struct multiboot_mmap_entry {
    uint32_t size;       // size of this entry (not including this field)
    uint64_t addr;       // base physical address
    uint64_t len;        // length in bytes
    uint32_t type;       // memory type
} __attribute__((packed)) multiboot_mmap_entry_t;



//! 8 blocks per byte
#define PMMNGR_BLOCKS_PER_BYTE 8

//! block size (4k)
#define PMMNGR_BLOCK_SIZE 4096

//! block alignment
#define PMMNGR_BLOCK_ALIGN PMMNGR_BLOCK_SIZE

//! extra pages added to kernel_end
//! compiler adds extra sections, maybe note: look more into this
#define KERNEL_SAFE_PAGES 2
int mmap_first_free();

void pmmngr_init(size_t memSize);

void pmmngr_init_region(physical_addr base, size_t size);

void pmmngr_deinit_region(physical_addr base, size_t size);

void parse_boot_map(uint32_t mmap_addr, uint32_t mmap_length);