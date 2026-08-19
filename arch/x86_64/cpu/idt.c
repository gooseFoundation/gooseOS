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

#include <arch/cpu/int.h>
#include <arch/cpu/idt.h>

// This is the actual IDT table, its filled with 256 IDT entries
// The CPU uses it to find the interrupt handlers
struct idt_entry idt_table[256];

// This is the pointer structure used by CPU to find the IDT table
// Its the same as the GDT pointer structure!
struct idt_ptr idt_pointer;

// Defualt IDT interrupt handler function defintion
typedef void (*idt_handler_t)(void);

// These are all the ISR handlers neatly inside of a list, we are gonna use this list to set the IDT entries
// Also just here for convinience so we can use a FOR loop for these
idt_handler_t isrHandlers[31] = {
    isr0, isr1, isr2, isr3, isr4, isr5, isr6, isr7, isr8, isr9, isr10,
    isr11, isr12, isr13, isr14, isr15, isr16, isr17, isr18, isr19, isr20,
    isr21, isr22, isr23, isr24, isr25, isr26, isr27, isr28, isr29, isr30,
    isr31
};


/// @brief Sets the IDT entry at I to use the provided handler at HANDLR
/// @param i: Index of entry to set
/// @param handlr: Handler to use for entry
void idt_set_entry(uint16_t i, uintptr_t handlr) {
    uint64_t addr = (uint64_t)handlr;

    // Handler base
    idt_table[i].base_low  = addr & 0xFFFF;
    idt_table[i].base_mid  = (addr >> 16) & 0xFFFF;
    idt_table[i].base_high = (addr >> 32);

    idt_table[i].sel = 0x08; // Selector in GDT
    idt_table[i].ist = 0; // IST table id(0)
    idt_table[i].flags = 0x8E; // Type and atributes
    idt_table[i].zero = 0; // Reserved(allways zero)
}

/// @brief Sets the SIZE amount of entries starting from START and onward
/// @param handlers: Handlers to use
/// @param size: How many handlers and in the table
/// @param start: Where in the IDT table should the first entry be?
void idt_set_entries_from_array(idt_handler_t handlers[], uint8_t size, uint8_t start) {
    // This is a pretty simple function, all we need to do is
    // use a FOR loop thru all of the specified handlers!
    for (uint8_t i = 0; i < size; i++) {
        idt_set_entry(i + start, (uintptr_t)handlers[i]);
    }
}

/// @brief Initilizes the interrupt descriptor table and registers the handlers
/// @note To load the IDT, call the idt_load function!
/// @internal This function is internal to the CPU subsystem of the x86_64 port for GooseOS!
void idt_init() {
    idt_set_entries_from_array(isrHandlers, 32, 0); // Set ISRs

    // Since we dont have an actual interrupt setup(with IRQs)
    // We can't actually recive keyboard or mouse yet! Our only interrupts are actually just exceptions
}

/// @brief Loads the interrupt descriptor table(IDT), should be called after "idt_init()" or setting a new entry!
void idt_load() {
    // Create the generic pointer structure, this is also the same format used for the GDT!
    idt_pointer.limit = (sizeof(struct idt_entry) * 256) - 1; 
    idt_pointer.base = (uintptr_t)idt_table;

    // Use LIDT to tell the CPU to load our IDT table
    // Make sure to allways load "idt_pointer" instead of "idt_table"
    asm volatile("lidt (%0)" : : "r" (&idt_pointer));
}