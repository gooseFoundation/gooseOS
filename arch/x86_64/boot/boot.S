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
.global _start
.extern _load_gdt
.extern _c_entry

# Halts the CPU forever, never returns! Don't expect any code to run after this! 
_hlt:
    cli
.L_hlt_loop:
    hlt
    jmp .L_hlt_loop

# Main entry function, called by our bootloader and used to setup the kernel
# Also the VERY FIRST function in the entire OS!
_start:
    cli # Disable interrupts(since we havent gotten the IDT set up yet)

    # Now we can set the stack for our C code, this is important as bootloader stacks are usually not permenant!
    # On x86_64 the register for the stack pointer is called RSP, we just need to set RSP to our stack bottom since it grows downwards!
    #mov _kstack_end, rsp

    call _load_gdt # Load the GDT required for well anything
    call _c_entry # Call the kernel entry function(k_entry)

    # This code only executes if the kernel entry returns, that means a fatal crash or just a critical bug
    # So we MUST halt instantly and stop all exection
    jmp _hlt

# .equ is a calculation thats done at compile time, its used for simple math like our stack size!
.equ _stack_size, 1024 * 4 # Calculate the stack size(for now it's 4 KiB)

.section .bss
.align 16
    _kstack_start:
        .skip _stack_size
    _kstack_end: