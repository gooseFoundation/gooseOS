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
#include <asm/io.h>
#include <drivers/kcon.h>

uint16_t uart_port = 0;

/// @brief: Initilizes the UART interface at the provided port(anywhere from COM1-7)
/// @param port: What COM interface to use?
void uart_init(uint16_t port) {
    uart_port = port;

    outb(port + 1, 0x00);    // Disable all interrupts
    outb(port + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(port + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb(port + 1, 0x00);    //                  (hi byte)
    outb(port + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(port + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(port + 4, 0x0B);    // IRQs enabled, RTS/DSR set
    outb(port + 4, 0x1E);    // Set in loopback mode, test the serial chip
    outb(port + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)
    
    // Check if the port is faulty, by checking if the same byte is recived after sent
    if(inb(port + 0) != 0xAE) {    
        kprintf("uart: Faulty serial controller at port %x!\n", port);
        return;       
    }

    outb(port + 4, 0x0F); // Now that we have confirmed its fine, lets set it back to normal mode
}

int is_transmit_empty() {
   return inb(uart_port + 5) & 0x20;
}

/// @brief Prints a character on the UART output
/// @param c: Character to print
void uart_putc(const char c) {
    while (is_transmit_empty() == 0); // waste a bunch of cycles

    outb(uart_port, c); // Write the character to serial after we confirm its safe
}