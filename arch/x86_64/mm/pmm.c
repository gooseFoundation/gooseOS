/*
 * ===========================================
 * Copyright(c) 2026-present IgosProjects
 * Copyright(c) 2026-present TheGooseFoundation and contributors
 * 
 * GooseOS is licensed under the GNU General Public License version 3(GPLv3), 
 * the license text can be found inside of the LICENSE file at the root of the repositoy
 *
 * If you didnt receive the LICENSE file with this source code, 
 * it can be found at https://www.gnu.org/licenses/gpl-3.0.html
 * ==========================================
 */ 

#include <mm/pmm.h>
#include <utils/boot/info.h>
#include <stdint.h>
#include <mm/mem.h>
#include <assert.h>
#include <drivers/kcon.h>

uint8_t* bitmap = NULL;
size_t total_pages = 0;
size_t bitmap_size = 0;

void pmm_setBit(size_t page_idx)   { bitmap[page_idx / 8] |=  (1 << (page_idx % 8)); }
void pmm_clearBit(size_t page_idx) { bitmap[page_idx / 8] &= ~(1 << (page_idx % 8)); }
bool pmm_testBit(size_t page_idx)  { return (bitmap[page_idx / 8] & (1 << (page_idx % 8))); }

/// @brief Initilizes the PMM(Physical Memory Manager)
void pmm_init(struct BootInfo* g_BootInfo) {
    uint64_t mapCount = g_BootInfo->memory_map_count;
    uint64_t maxAddr = 0;

    for (size_t i = 0; i < g_BootInfo->memory_map_count; i++) {
        struct GMemoryMapEntry entry = g_BootInfo->memory_map[i];
        kprintf("pmm: type: %u, base: %lx, length: %u\n", entry.type, entry.base, entry.length); // Print some debug info
    }

    // Find the top of usable memory(where we can allocate to and from)
    for (size_t i = 0; i < g_BootInfo->memory_map_count; i++) {
        struct GMemoryMapEntry* entry = &g_BootInfo->memory_map[i];
        if (entry->type == GOS_MEMMAP_USABLE) {
            uint64_t top = entry->base + entry->length;
            if (top > maxAddr) maxAddr = top;
        }
    }

    kprintf("pmm: top of RAM: %lx\n", maxAddr); // Print debug info

    // Calculate the amount of pages and the size of the bitmap
    total_pages = maxAddr / 4096;
    bitmap_size = (total_pages + 7) / 8;

    // Find a chunk of RAM that can hold our bitmap
    uint64_t bitmap_phys_addr = 0;
    for (size_t i = 0; i < g_BootInfo->memory_map_count; i++ ) {
        struct GMemoryMapEntry* entry = &g_BootInfo->memory_map[i];
        
        // Make sure we arent in the first 4 KB region(BIOS data and IVT(legacy x86 interrupt table))
        if (entry->type == GOS_MEMMAP_USABLE && entry->length >= bitmap_size && entry->base > 0) {
            bitmap_phys_addr = entry->base;
            break;
        }
    }

    // Oh no! We didnt find a spot for our bitmap!
    // We wanna just throw a good ol bugcheck since we cant continue
    ASSERT(bitmap_phys_addr != 0);

    bitmap = (uint8_t*)(bitmap_phys_addr + g_BootInfo->hhdm_offset);
    kprintf("pmm: bitmap is from now stored at: %lx\n", bitmap); // Print debug info

    // Use memset to lock EVERYTHING in the bitmap for now
    memset(bitmap, 0xFF, bitmap_size);

    // Free all the usable addresses
    for (size_t i = 0; i < g_BootInfo->memory_map_count; i++) {
        if (g_BootInfo->memory_map[i].type == GOS_MEMMAP_USABLE) {
            size_t start_page = g_BootInfo->memory_map[i].base / 4096;
            size_t page_count = g_BootInfo->memory_map[i].length / 4096;
            for (size_t p = 0; p < page_count; p++) {
                pmm_clearBit(start_page + p);
            }
        }
    }

    pmm_setBit(0); // Lock page 0 because returning NULL is bad i guess!

    // Mark the bitmap address as USED so we dont overmark ourselves
    size_t bitmap_start_page = bitmap_phys_addr / 4096;
    size_t bitmap_page_count = (bitmap_size + 4095) / 4096;
    for (size_t p = 0; p < bitmap_page_count; p++) {
        pmm_setBit(bitmap_start_page + p);
    }

    // Protect the real mode memory(below 1MiB)
    for (size_t page = 0; page < 256; page++) {
        pmm_setBit(page);
    }

    // Protect bootloader and kernel data
    for (size_t i = 0; i < g_BootInfo->memory_map_count; i++) {
        struct GMemoryMapEntry* entry = &g_BootInfo->memory_map[i];

        // If it's NOT usable, OR if it's the kernel/modules/reclaimable memory, lock it up!
        if (entry->type != GOS_MEMMAP_USABLE) {
            size_t start_page = entry->base / 4096;
            // Round up page count to ensure full coverage of unaligned regions
            size_t page_count = (entry->length + 4095) / 4096;
            
            for (size_t p = 0; p < page_count; p++) {
                if ((start_page + p) < total_pages) {
                    pmm_setBit(start_page + p);
                }
            }
        }
    }
}

/// @brief Allocates a 4 KiB page in memory and returns it
/// @return Pointer to start of allocated section
uintptr_t pmm_alloc_page() {
    // Attempt to allocate a page
    for (size_t i = 0; i < total_pages; i++) {
        if (!pmm_testBit(i)) {
            pmm_setBit(i);
            return (i * 4096); // Get the raw address and return it
        }
    }

    // This code runs if we CAN'T allocate the page, so we gotta panic!
    k_bugcheck("pmm: Out of memory");
    return 0;
}