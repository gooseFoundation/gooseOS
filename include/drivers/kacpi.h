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

/// @brief This is the generic ACPI header that is used on every table
/// @brief Every ACPI table must start with one of these!
struct __attribute__((packed)) acpi_table_header {
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
};

struct __attribute__((packed)) acpi_rsdp {
    char signature[8];
    uint8_t checksum;
    char oem_id[6];
    uint8_t revision;
    uint32_t rsdt_address; // Legacy 32 bit pointer, do not use!
    
    // ACPI 2.0+ fields (64-bit safe)
    uint32_t length;
    uint64_t xsdt_address; // Address in PHYSICAL memory of root table!
    uint8_t extended_checksum;
    uint8_t reserved[3];
};

/// @brief The XSDT is the modernized root ACPI pointer table
/// @brief Its the "modernized" version of the old 32 bit system
struct __attribute__((packed)) acpi_xsdt {
    struct acpi_table_header hdr; // ACPI table header(required on every table!)
    uint64_t tables[]; // This is the array of 64 bit pointers that all ACPI tables are stored in, this array has a pointer to EVERY ACPI table on this machine!
};

/// @brief Initilizes the ACPI subsystem, required otherwise nothing will work!
/// @param xsdt_ptr 
void acpi_init(uintptr_t rsdp_ptr, uintptr_t hhdm_offset);

/// @brief Attempts to find the ACPI table based on the signature
/// @param signature: The ACPI table signature
/// @return: Pointer to table
struct acpi_table_header* acpi_get_table_by_sig(char signature[4]);