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

#include <kernel/conf.h>
#include <utils/boot/info.h>
#include <stdbool.h>
#include <assert.h>

// Tell the linker that we want the kernel entry point
extern void k_entry(struct BootInfo* boot_info);

// If the users wants the limine bootloader, we have to include teh eheaders
#if CONF_USE_LIMINE == 1
    #include <utils/boot/limine.h>
    
    extern struct limine_framebuffer_request framebuffer_request; // Framebuffer
    extern struct limine_memmap_request memmap_request; // MemMap
    extern struct limine_hhdm_request hhdm_request;
    extern struct limine_rsdp_request rsdp_request;
#endif

/// @brief Switches to false after early boot is done, meaning that the console and other services exist
bool k_InEarlyBoot = true;

// We dont have a dynamic allocator or "kmalloc" inside of early boot
// So we need some static arrays inside of BSS or wherever the linker puts variables
static struct GFramebuffer g_framebuffers[8];
static struct GFramebuffer* g_fb_ptrs[8];
static struct GMemoryMapEntry g_memmap_entries[256]; // @bug: Make it support more than 256 entries
static struct BootInfo g_boot_info;

// Early boot entry called by boot.S, runs before even the kernel starts!
// Used to translate for example the Limine protocol into our custom BootInfo
void _c_entry() {
    if (!k_InEarlyBoot) k_InEarlyBoot = true;

    // Framebuffer setup
    #if CONF_USE_LIMINE == 1
        // Get the framebuffer response from limine and also check it for being valid
        struct limine_framebuffer_response* fb_response = framebuffer_request.response;

        // Check NULL FIRST
        if (!fb_response) {
            while(1) __asm__("hlt");
        }

        // Now it's safe to access
        if (fb_response->framebuffer_count == 0) {
            while(1) __asm__("hlt");
        }
        
        // Get the count of framebuffers, we support a max of 8 for now!
        uint64_t fb_count = fb_response->framebuffer_count;
        if (fb_count > 8) fb_count = 8;

        // Convert all the framebuffers from Limine's format into our custom GFramebuffer format
        for (uint64_t i = 0; i < fb_count; i++) {
            struct limine_framebuffer *limine_fb = fb_response->framebuffers[i];
        
            g_framebuffers[i].address = (uintptr_t)limine_fb->address;
            g_framebuffers[i].width = limine_fb->width;
            g_framebuffers[i].height = limine_fb->height;
            g_framebuffers[i].pitch = limine_fb->pitch;
            g_framebuffers[i].bpp = limine_fb->bpp;
            g_framebuffers[i].red_mask_size = limine_fb->red_mask_size;
            g_framebuffers[i].red_mask_shift = limine_fb->red_mask_shift;
            g_framebuffers[i].green_mask_size = limine_fb->green_mask_size;
            g_framebuffers[i].green_mask_shift = limine_fb->green_mask_shift;
            g_framebuffers[i].blue_mask_size = limine_fb->blue_mask_size;
            g_framebuffers[i].blue_mask_shift = limine_fb->blue_mask_shift;
        
            g_fb_ptrs[i] = &g_framebuffers[i];
        }

        // Now we can set the framebuffer values inside of the the boot info struct
        g_boot_info.framebuffers = g_fb_ptrs;
        g_boot_info.fb_count = fb_count;
    #endif

    // Setup the memory map(MemMap)
    #if CONF_USE_LIMINE
        struct limine_memmap_response* memmap_response = memmap_request.response;

        // Check for NULL
        ASSERT(memmap_response);

        uint64_t memmap_count = memmap_response->entry_count;
        ASSERT(memmap_count < 256);

        // Now we need to actually read all of the memory map entries into our custom format
        // This is very simple but we will need some hacks
        for (uint64_t i = 0; i < memmap_count; i++) {
            g_memmap_entries[i].base = memmap_response->entries[i]->base;
            g_memmap_entries[i].length = memmap_response->entries[i]->length;
            g_memmap_entries[i].type = memmap_response->entries[i]->type;
        }
        
        // Insert it into BootInfo
        g_boot_info.memory_map = g_memmap_entries;
        g_boot_info.memory_map_count = memmap_count;

    #endif

    #if CONF_USE_LIMINE
        g_boot_info.hhdm_offset = hhdm_request.response->offset; // yes, all thats translated lol
        g_boot_info.rsdp_addr = (uintptr_t)rsdp_request.response->address; // NOTICE: this piece of shit is so buggy, pls fix?
    #endif

    k_entry(&g_boot_info); // Now lets actually call the kernel entry
}