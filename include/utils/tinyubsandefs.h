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
#include <kernel/panic.h>
#include <drivers/kcon.h>

// Define TinyUbsan functions
#define TINYUBSAN_PRINT kprintf // Make sure TinyUbsan uses our console driver

/// @brief Internally used function, called by TinyUbsan when it detects an error!
static inline void _ubsan_catch() {
    k_bugcheck("LibUbsan triggered fault!(look above)");
}

#define TINYUBSAN_TRAP _ubsan_catch