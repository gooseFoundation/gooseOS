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

struct idt_entry {
    uint16_t base_low;
    uint16_t sel;
    uint8_t ist;
    uint8_t flags;
    uint16_t base_mid;
    uint32_t base_high;
    uint32_t zero;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

/// @brief Initilizes the interrupt descriptor table and registers the handlers
/// @note To load the IDT, call the idt_load function!
/// @internal This function is internal to the CPU subsystem of the x86_64 port for GooseOS!
void idt_init();

/// @brief Loads the interrupt descriptor table(IDT), should be called after "idt_init()" or setting a new entry!
void idt_load();

/// @brief Sets the IDT entry at I to use the provided handler at HANDLR
/// @param i: Index of entry to set
/// @param handlr: Handler to use for entry
void idt_set_entry(uint16_t i, uintptr_t handlr);