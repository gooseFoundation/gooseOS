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

/// @brief Initilizes the allocator and makes sure that its safe to provide "kmalloc" and "kfree"
void alloc_init();

/// @brief Allocates some memory in the global heap
/// @param amount: Amount of memory to allocate
/// @return: Pointer to allocated segment
uintptr_t kmalloc(size_t amount);