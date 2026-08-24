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
#include <mm/alloc.h>
#include <mm/vmm.h>

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
    vmm_init(boot_info->hhdm_offset);
    pmm_init(boot_info);
    alloc_init();

    k_InEarlyBoot = false;

    // Run some tests
    kprintf("test: Testing kmalloc\n");
    kprintf("test: Allocated 1000 bytes of memory at 0x%x\ntest: Allocated 2000 bytes of memory at 0x%x\ntest: Allocated 4 KiB of memory at 0x%x", kmalloc(1000), kmalloc(2000), kmalloc(4096));
    kprintf("test: wow!\n");

    for (;;) {
        asm volatile("hlt");
    }
}
