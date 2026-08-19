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
#include <utils/boot/info.h>
#include <stdarg.h>

/// @brief Initilizes the console driver allowing for characters to be printed to the display
/// @attention Call only ONCE on boot!
void kcon_init(struct GFramebuffer* fb);

/// @brief Prints a single character to the display, replacement for the standard C function "putc"
/// @param c: The character to print
void kputc(const char c);

/// @brief Prints a formatted string to the display, replacement for the standard C function "printf"
/// @param str: The string to print to the display
/// @param ...: Formatting to use
void kprintf(const char* str, ...);

/// @brief Same as "kprintf()" but it accepts a direct VAList
/// @param str: The formatted string to print
/// @param args: Arguments to use 
void kvsnprintf(const char* str, va_list args);