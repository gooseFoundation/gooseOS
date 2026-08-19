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

/// @brief: Initilizes the UART interface at the provided port(anywhere from COM1-7)
/// @param port: What COM interface to use?
void uart_init(uint16_t port);

/// @brief Prints a character on the UART output
/// @param c: Character to print
void uart_putc(const char c);