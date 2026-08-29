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
#include <stdbool.h>

extern bool k_InEarlyBoot;

#define GOS_MEMMAP_USABLE 0
#define GOS_MEMMAP_RESERVED 1
#define GOS_MEMMAP_ACPI_RECLAIMABLE 2
#define GOS_MEMMAP_ACPI_NVS 3
#define GOS_MEMMAP_BAD_MEMORY 4
#define GOS_MEMMAP_BOOTLOADER_RECLAIMABLE 5
#define GOS_MEMMAP_EXECUTABLE_AND_MODULES 6
#define GOS_MEMMAP_FRAMEBUFFER 7
#define GOS_MEMMAP_RESERVED_MAPPED 8

/// @brief MMap entry
struct GMemoryMapEntry {
    uint64_t base;
    uint64_t length;
    uint64_t type;
};

/// @brief Standard framebuffer struct used around GooseOS
/// @note Based off of the Limine framebuffer struct
struct GFramebuffer {
    uintptr_t address; // Offset from the start of memory to the framebuffer

    // Height and width
    uint64_t height;
    uint64_t width;

    uint64_t pitch; // Bytes per row of pixels, used for drawing logic
    uint16_t bpp; // Amount of bits per each pixel

    // Red mask
    uint8_t red_mask_size;
    uint8_t red_mask_shift;

    // Green mask
    uint8_t green_mask_size;
    uint8_t green_mask_shift;

    // Blue mask
    uint8_t blue_mask_size;
    uint8_t blue_mask_shift;
};

struct BootInfo {
    // Framebuffer
    struct GFramebuffer** framebuffers;
    uint64_t fb_count; // Amount of framebuffers that are currently avilable

    // MemMap
    struct GMemoryMapEntry* memory_map;
    uint64_t memory_map_count;

    // HHDM address(where memory is mapped in the page table)
    uint64_t hhdm_offset; // by defualt NULL, if translator doesnt fill in, the stuff will break!

    // RSDP address(ACPI root table)
    uintptr_t rsdp_addr;
};