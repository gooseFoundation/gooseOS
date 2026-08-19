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

#include <drivers/cpu.h>
#include <arch/cpu/idt.h>

/// @brief Initilizes all the CPU stuff and architecure specific things
/// @important Call only ONCE on boot!
void cpu_init() {
    // IDT init
    idt_init();
    idt_load();
}