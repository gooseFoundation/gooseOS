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

#pragma once
#include <stdint.h>

/// @brief Used to define VMM page flags, each VMM has its own conversion layer!
/// @note Not required on kmalloc!
enum GOS_PAGE_FLAGS {
    GOS_PAGE_READ = 1 << 0, // Can this page be read?
    GOS_PAGE_WRITE = 1 << 1, // Can this page be written to?
    GOS_PAGE_USER = 1 << 2 // Can the userspace code access this page?
};

/// @brief Maps a physical address in memory to a virtual address
/// @param phys_addr: Physical address to map
/// @param addr: Virtual address to use
/// @param flags: Flags to use(ReadOnly, User, Active)
void vmm_map_addr(uintptr_t phys_addr, uintptr_t addr, uint32_t flags);

/// @brief Unmaps the provided virtual address making any writes and reads to it a fault!
/// @bug This is not really a bug, but using this address afterward WILL kill the OS!
/// @param addr: Address to unmap 
void vmm_unmap_addr(uintptr_t addr);

/// @brief Initilizes the VMM and allows for mapping and unmapping of pages
/// @param hhdm_offset: HHDM offset used for physical to virtual conversion
void vmm_init(uintptr_t hhdm_offset);