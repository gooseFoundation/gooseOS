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

.intel_syntax noprefix

.section .text
.code64

.global outb
.global inb
.global outw
.global inw

# Send a 8 bit value down a port
outb:
    mov dx, di # Read port argument into dx
    mov al, sil # Read value argument into al
    out dx, al
    ret # Return from function

# Read a 8 bit value from a port
inb:
    mov dx, di # Read port argument into dx
    in al, dx # Read into al
    movzx eax, al
    ret # Return from function

# Send a 16 bit value down a port
outw:
    mov dx, di # Read port value
    mov ax, si # Read value
    out dx, ax
    ret # Return from function

# Read a 16 bit value from a port
inw:
    mov dx, di # Read port value
    in ax, dx
    ret # Return from function