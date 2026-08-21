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

#include <stdint.h>
#include <mm/pmm.h>
#include <mm/vmm.h>
#include <drivers/kcon.h>

/// @brief Internally used permissions for the x86_64 page tables
enum _x86_internal_PTE_permissions {
    PTE_PRESENT = (1ULL << 0),
    PTE_WRITABLE = (1ULL << 1),
    PTE_USER = (1ULL << 2)
};

uintptr_t _vmm_hhdm_offset = 0; // HHDM offset, keep at zero if the system doesnt use a HHDM!

// Finds the next table in the page table
uint64_t* _x86_get_next_table(uint64_t* current_table, size_t index, uint64_t hhdm_offset) {
    if (!(current_table[index] & PTE_PRESENT)) {
        uint64_t new_table_phys = (uint64_t)pmm_alloc_page();
        if (!new_table_phys) {
            return 0;
        }

        uint8_t* new_table_virt = (uint8_t*)(new_table_phys + hhdm_offset);
        for (int i = 0; i < 4096; i++) {
            new_table_virt[i] = 0;
        }

        current_table[index] = new_table_phys | PTE_PRESENT | PTE_WRITABLE;
    }

    return (uint64_t*)((current_table[index] & 0x000FFFFFFFFFF000ULL) + hhdm_offset);
}


/// @brief Converts the GooseOS page flags(GOS_PAGE_*) into x86_64 PTE permissions
/// @internal Not meant to be used outside of the VMM!
/// @param flags: Flags to convert into PTE
/// @return PTE permissions
uint64_t _x86_page_flags_converter(uint32_t flags) {
    uint64_t returned_permissions = 0;

    kprintf("vmm: Page Permissions: \n");
    if (flags & GOS_PAGE_READ) {
        kprintf("vmm: READ\n");
        returned_permissions |= PTE_PRESENT; // A page with Present is allways a readable page!
    }

    if (flags & GOS_PAGE_WRITE) {
        kprintf("vmm: WRITE\n");
        returned_permissions |= PTE_WRITABLE;
    }

    if (flags & GOS_PAGE_USER) {
        kprintf("vmm: USER\n");
        returned_permissions |= PTE_USER;
    }

    return returned_permissions;
}

/// @brief Returns the PML4 address based on the CR3
/// @return PML4 pointer
uint64_t _x86_get_cr3() {
    uint64_t cr3;

    asm volatile ("mov %%cr3, %0" : "=r"(cr3));

    return (cr3 & ~0xFFF) + _vmm_hhdm_offset;
}

/// @brief Maps a physical address in memory to a virtual address
/// @param phys_addr: Physical address to map
/// @param addr: Virtual address to use
/// @param flags: Flags to use(ReadOnly, User, Active)
void vmm_map_addr(uintptr_t phys_addr, uintptr_t addr, uint32_t flags) {
    if (!phys_addr || !addr || !flags) {
        kprintf("vmm: invalid call to map_addr!\n");
        return;
    }

    // First, we need to convert the flags from GooseOS format to the x86 PTE format
    // This is just 1 function call thankfully!
    uint64_t page_flags = _x86_page_flags_converter(flags);

    // Now lets get all of the indexes of things
    // We are gonna need this later
    size_t pml4_idx = (addr >> 39) & 0x1FF;
    size_t pdpt_idx = (addr >> 30) & 0x1FF;
    size_t pd_idx   = (addr >> 21) & 0x1FF;
    size_t pt_idx   = (addr >> 12) & 0x1FF;

    uintptr_t pml4_virt = _x86_get_cr3();

    // Get the page tables from PML4
    uint64_t* pdpt = _x86_get_next_table((uint64_t*)pml4_virt, pml4_idx, _vmm_hhdm_offset);
    uint64_t* pd   = _x86_get_next_table(pdpt, pdpt_idx, _vmm_hhdm_offset);
    uint64_t* pt   = _x86_get_next_table(pd, pd_idx, _vmm_hhdm_offset);

    // Insert the entry into the PT, now all thats left is to make the CPU forget the old mapping
    pt[pt_idx] = (phys_addr & 0x000FFFFFFFFFF000ULL) | page_flags | PTE_PRESENT;

    // Whoever designed these names is drunk idk, WHAT IS INVLPG?
    asm volatile("invlpg (%0)" :: "r"(addr) : "memory");
}

/// @brief Initilizes the VMM and allows for mapping and unmapping of pages
/// @param hhdm_offset: HHDM offset used for physical to virtual conversion
void vmm_init(uintptr_t hhdm_offset) {
    if (!hhdm_offset) {kprintf("vmm: Invalid HHDM passed to init!"); return;}

    _vmm_hhdm_offset = hhdm_offset; // Set the offset after checking
}