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
#include <utils/boot/info.h>
#include <stdint.h>
#include <stddef.h>

/// @brief Initilizes the PMM(Physical Memory Manager)
void pmm_init(struct BootInfo* g_BootInfo);

/// @brief Allocates a 4 KiB page in memory and returns it
/// @return Pointer to start of allocated section
uintptr_t pmm_alloc_page();