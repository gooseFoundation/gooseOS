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
#include <mm/vmm.h>
#include <kernel/conf.h>
#include <assert.h>
#include <drivers/kcon.h>
#include <stdint.h>

// This is the main allocator implementation,
// it implements a basic bump allocator inspired by other OS projects!
// TODO: Make it so the bump allocator is only used on early boot

extern const char k_end[]; // Filled in by the linker script, this is how we know where its safe for our heap to go!

// NOTICE: The below values are all filled in later on,
// they are also the biggest crash reason if someone allocates before "alloc_init()"
uintptr_t alloc_heap_start = (uintptr_t)k_end;
uintptr_t alloc_heap_end = 0;
uintptr_t alloc_heap_current_ptr = 0;

static uintptr_t _align_up(uintptr_t value, uintptr_t alignment) {
    return (value + alignment - 1) & ~(alignment - 1);
}

static uintptr_t _align_down(uintptr_t value, uintptr_t alignment) {
    return value & ~(alignment - 1);
}

/// @brief Initilizes the allocator and makes sure that its safe to provide "kmalloc" and "kfree"
void alloc_init() {
    //kprintf("alloc: k_end: 0x%x\n", (uintptr_t)k_end);
    //kprintf("alloc: heap start: 0x%x\n", alloc_heap_start);

    const uintptr_t alloc_heap_start_phys = pmm_alloc_page();  // Allocate a new page that will be the start of our heap. We will use this later
    //ASSERT(!heap_start_page && heap_start_page == NULL); // Check if we recived the pointer

    alloc_heap_start = _align_up((uintptr_t)k_end, 4096) + 0x100000;

    // Now we need to map it into the page tables
    // If not, we will fault!
    vmm_map_addr(alloc_heap_start_phys, _align_up((uintptr_t)k_end, 4096), GOS_PAGE_READ | GOS_PAGE_WRITE);
    alloc_heap_current_ptr = alloc_heap_start;

    //kprintf("alloc: Allocated heap start page at: 0x%x\n", alloc_heap_start);

    // We need to convert from KiB to bytes,
    // this is possible using simple math! Then we need to split it into 4 KiB pages

    const uint32_t heap_needed_pages = (CONF_MEM_HEAP_SIZE * 1024) / 4096;
    uintptr_t heap_last_page = alloc_heap_start;

    kprintf("alloc: Required pages for heap: %d\n", heap_needed_pages);

    // Allocate all the needed pages for the heap
    for (uint64_t PageIndex = 0; PageIndex < heap_needed_pages - 1; PageIndex++) {
        const uintptr_t heap_new_page_phys = pmm_alloc_page();
        vmm_map_addr(heap_new_page_phys, heap_last_page + (4 * 1024), GOS_PAGE_READ | GOS_PAGE_WRITE);

        uintptr_t heap_new_page_virt = heap_last_page + (4 * 1024);

        heap_last_page = heap_new_page_virt;
        kprintf("malloc: Allocated new heap page at %x\n", heap_last_page);
        kprintf("alloc: new heap page phys: 0x%x, new heap page virt: 0x%x\n", heap_new_page_phys, heap_new_page_virt);
    }

    alloc_heap_end = heap_last_page + (4 * 1024); // FIX: "heap_last_page" is the pointer to the START of the page, NOT THE END!
}

/// @brief Allocates some memory in the global heap
/// @param amount: Amount of memory to allocate
/// @return: Pointer to allocated segment
uintptr_t kmalloc(size_t amount) {
     if (amount == 0) {kprintf("alloc: Attempt to allocate 0 bytes, please dont!\n"); return 0; }
    if (alloc_heap_current_ptr > alloc_heap_end) k_bugcheck("alloc: alloc_heap_current_ptr is OUT OF BOUNDS!");

    // FIX: Fix security issue featured in https://git.evalyngoemer.com/IgosProjects1/gooseOS/issues/1
    const size_t remamingSpace = alloc_heap_end - alloc_heap_current_ptr;

    // If we are overflowing the heap, we need to grow the heap forward
    if (remamingSpace < amount) {
        const size_t needed_bytes = amount - remamingSpace;
        const size_t heap_needed_pages = (needed_bytes + 4095) / 4096;
        uintptr_t heap_last_page = alloc_heap_end;

        kprintf("alloc: Required pages for growing heap: %d\n", heap_needed_pages);

        // Allocate all the needed pages for growing the page
        for (uint64_t PageIndex = 0; PageIndex < heap_needed_pages; PageIndex++) {
            const uintptr_t heap_new_page_phys = pmm_alloc_page();
            vmm_map_addr(heap_new_page_phys, heap_last_page, GOS_PAGE_READ | GOS_PAGE_WRITE);

            uintptr_t heap_new_page_virt = heap_last_page + (4 * 1024);

            heap_last_page = heap_new_page_virt;
            kprintf("malloc: Allocated new heap page at %x\n", heap_last_page);
        }
 
        alloc_heap_end = heap_last_page;
    }

    // Move forward the pointer and return the start
    // We dont enforce anything so the user is responsible for NOT fucking up memory!
    alloc_heap_current_ptr += amount;

    // And now return it
    return (alloc_heap_current_ptr - amount);
}

/// @brief Frees the memory at the provided pointer
/// @param ptr: Pointer to free 
void kfree(uintptr_t ptr) {
    kprintf("alloc: kfree is unsupported!\n");
}