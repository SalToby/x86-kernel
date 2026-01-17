#include "mmngr_phys.h"
#include "print.h"

//! size of physical memory
static	uint32_t _mmngr_memory_size=0;
 
//! number of blocks currently in use
static	uint32_t _mmngr_used_blocks=0;
 
//! maximum number of available memory blocks
static	uint32_t _mmngr_max_blocks=0;
 
//! memory map bit array. Each bit represents a memory block
static	uint32_t* _mmngr_memory_map= 0;

static inline void mmap_set (int bit) { 
  _mmngr_memory_map[bit / 32] |= (1 << (bit % 32));
}

static inline void mmap_unset (int bit) {
    _mmngr_memory_map[bit / 32] &= ~ (1 << (bit % 32));
}

static inline bool mmap_test (int bit) {
    return _mmngr_memory_map[bit / 32] &  (1 << (bit % 32));
}

static inline uint32_t pmmngr_get_block_count() {
    return _mmngr_max_blocks;
}

static inline uint32_t pmmngr_get_memory_size() {
    return _mmngr_memory_size;
}


int mmap_first_free() {
 
	//! find the first free bit
	for (uint32_t i=0; i< pmmngr_get_block_count() / 32; i++)
		if (_mmngr_memory_map[i] != 0xffffffff)
			for (int j=0; j<32; j++) {		//! test each bit in the dword
 
				int bit = 1 << j;
				if (! (_mmngr_memory_map[i] & bit) )
					return i*4*8+j;
			}
 
	return -1;
}

void pmmngr_init(size_t memSize) {
	uintptr_t bitmap = PAGE_ALIGN_UP(_kernel_end + KERNEL_SAFE_PAGES * PMMNGR_BLOCK_SIZE);

	_mmngr_memory_size	=	memSize;
	_mmngr_memory_map	=	(uint32_t*) bitmap;
	_mmngr_max_blocks	=	(pmmngr_get_memory_size()*1024) / PMMNGR_BLOCK_SIZE;
	_mmngr_used_blocks	=	pmmngr_get_block_count();
 
	//! By default, all of memory is in use
	memset(_mmngr_memory_map, 0xff, pmmngr_get_block_count() / PMMNGR_BLOCKS_PER_BYTE );
}

void pmmngr_init_region(physical_addr base, size_t size) {
 
	int align = base / PMMNGR_BLOCK_SIZE;
	int blocks = size / PMMNGR_BLOCK_SIZE;
 
	for (; blocks>0; blocks--) {
		mmap_unset(align++);
		_mmngr_used_blocks--;
	}
 
	mmap_set(0);	//first block is always set. This insures allocs cant be 0
}

void pmmngr_deinit_region(physical_addr base, size_t size) {
 
	int align = base / PMMNGR_BLOCK_SIZE;
	int blocks = size / PMMNGR_BLOCK_SIZE;
 
	for (; blocks>0; blocks--) {
		mmap_set(align++);
		_mmngr_used_blocks++;
	}
}

void parse_boot_map(uint32_t mmap_addr, uint32_t mmap_length) {
    uintptr_t end = mmap_addr + mmap_length;
	uintptr_t kernel_phys_end = PAGE_ALIGN_UP(_kernel_end + KERNEL_SAFE_PAGES * PMMNGR_BLOCK_SIZE);


	
    // Cast the mmap_addr to pointer to multiboot_mmap_entry_t
    multiboot_mmap_entry_t* entry = (multiboot_mmap_entry_t*)mmap_addr;

    while ((uintptr_t)entry < end) {
		if (entry->type == 1) { // usable RAM

            uintptr_t region_start = (uintptr_t)entry->addr;
            uintptr_t region_end   = region_start + (uintptr_t)entry->len;

            // Clip anything below kernel+bitmap
            if (region_end <= kernel_phys_end) {
                // Entire region is below kernel → ignore
            }
            else {
                if (region_start < kernel_phys_end)
                    region_start = kernel_phys_end;

                // Page align
                region_start = PAGE_ALIGN_UP(region_start);
                region_end   = PAGE_ALIGN_DOWN(region_end);

                if (region_start < region_end) {
                    size_t size = region_end - region_start;
                    pmmngr_init_region(region_start, size);
                }
            }
        }

        // Move to the next entry
        entry = (multiboot_mmap_entry_t*)((uintptr_t)entry + entry->size + sizeof(entry->size));
    }
}