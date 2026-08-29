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
#include <stddef.h>
#include <assert.h>
#include <mm/mem.h>
#include <utils/boot/info.h>
#include <drivers/kcon.h>
#include <drivers/kacpi.h>

static uintptr_t global_xsdt_ptr = 0;
static uintptr_t global_hhdm_offset = 0;

/// @brief Initilizes the ACPI subsystem, required otherwise nothing will work!
/// @param xsdt_ptr 
void acpi_init(uintptr_t rsdp_ptr, uintptr_t hhdm_offset) {
    ASSERT(rsdp_ptr); // Check if the pointer is NULL
    ASSERT(hhdm_offset); // Also check if the HHDM offset is NULL
    
    kprintf("acpi: Recived RSDP ptr: 0x%lx\n", rsdp_ptr);
    kprintf("acpi: Recived HHDM offset: 0x%lx\n", hhdm_offset);

    struct acpi_rsdp* rsdp_table = (struct acpi_rsdp*)rsdp_ptr;

    ASSERT(!rsdp_table->revision < 2); // Check if this is ACPI 2.0
    ASSERT(rsdp_table->xsdt_address); // Check if we got the XSDT from the firmware(important!)
    
    global_xsdt_ptr = rsdp_table->xsdt_address + hhdm_offset;
    global_hhdm_offset = hhdm_offset; // Also set the HHDM offset
}

/// @brief Attempts to find the ACPI table based on the signature
/// @param signature: The ACPI table signature
/// @return: Pointer to table
struct acpi_table_header* acpi_get_table_by_sig(char signature[4]) {
    // Check if we are initalized?
    if (!global_xsdt_ptr) {k_bugcheck("acpi: Attempt to call 'acpi_get_table_by_sig()' before 'acpi_init()'!");}

    struct acpi_xsdt* xsdt_table = (struct acpi_xsdt*)global_xsdt_ptr; // Do some fancy C shit to convert into our struct
    size_t xsdt_lenght = (xsdt_table->hdr.length - sizeof(struct acpi_table_header)) / sizeof(uint64_t);

    // Loop thru all of the entries in the XSDT, this is how we find our table
    for (size_t i = 0; i < xsdt_lenght; i++) {
        struct acpi_table_header* table_header = (struct acpi_table_header*)(xsdt_table->tables[i] + global_hhdm_offset);

        if ((memcmp(signature, table_header->signature, 4)) == 0) {
            // We found our table!! Lets return it

            return table_header;
        }
    }
    
    return NULL; // We didnt find it, lets return NULL
}