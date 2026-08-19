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
#include <utils/boot/info.h>
#include <drivers/kcon.h>
#include <drivers/cpu.h>
#include <mm/pmm.h>

#include <extern/flanterm/flanterm.h>
#include <extern/flanterm/fb.h>

/// @brief Main kernel entry function
/// @internal Called internally by early boot init!
void k_entry(struct BootInfo* boot_info) {
    // Initilize the kernel console to print characters to the display
    // This is gonna be used in the future for logging to the screen!
    struct GFramebuffer* fb = boot_info->framebuffers[0];
    kcon_init(fb);

    // Now lets initlize all the other subsystems required for normal operation
    cpu_init();

    k_InEarlyBoot = false;
    pmm_init(boot_info);

    // Print "Hello, World!"
    kprintf("Hello, %s%c\n", "World", '!');

    for (;;) {
        asm volatile("hlt");
    }
}
