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

#include <utils/boot/info.h>
#include <kernel/panic.h>
#include <stdarg.h>
#include <asm/hcf.h>
#include <drivers/kcon.h>

extern bool K_InEarlyBoot;

/// @brief Triggers a kernel bug check(fancy name for kernel panic)
/// @note Stops the system forcefully, only call on critical issues!
void k_bugcheck(const char* reason, ...) {
    if (k_InEarlyBoot) {
        _halt_catch_fire(); // Use the generic halt function to stop the CPU
    }

    va_list args;
    va_start(args, reason);

    // Print the panic messages
    kprintf("Kernel Panic!\n");
    
    kprintf("\nGooseOS has encountered a FATAL error and HAS to quit!\nAny unsaved data might be lost!\n");
    kprintf("Please report this issue to our Github!\n");

    kvsnprintf(reason, args);
    va_end(args);

    _halt_catch_fire();
}