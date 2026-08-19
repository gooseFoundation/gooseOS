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
.global _isr_entry
.extern _isr_handler

# This is what the CPU calls when a fatal or nonfatal ISR exception occurs!
# Our IDT table is responsible for telling it to call it!
ISREntry:
    # Save registers
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov rbp, rsp # Save original RSP
    and rsp, -16 # Align RSP to the closest 16 bit value

    # Now that the stack if correctly made for C, we can call it without it crashing
    # That is also because the sysV x86_64 standard requires a 16 bit aligned stack!
    mov rdi, rbp # Push the stack to C
    call _isr_handler

    mov rsp, rbp # Restore RSP

    # Restore the registers
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    add rsp, 16 # Remove error code and interrupt number from stack
    iretq # Return from interrupt

.macro isr_err_stub n
    .global isr\n
isr\n:
    # CPU already pushed the error code
    push \n              # Push interrupt number
    jmp ISREntry
.endm

.macro isr_no_err_stub n
    .global isr\n
isr\n:
    push 0               # Push dummy error code
    push \n              # Push interrupt number
    jmp ISREntry
.endm

isr_no_err_stub 0 # ISR 0
isr_no_err_stub 1 # ISR 1
isr_no_err_stub 2 # ISR 2
isr_no_err_stub 3 # ISR 3
isr_no_err_stub 4 # ISR 4
isr_no_err_stub 5 # ISR 5
isr_no_err_stub 6 # ISR 6
isr_no_err_stub 7 # ISR 7
isr_err_stub 8 # ISR 8(double fault)
isr_no_err_stub 9 # ISR 9
isr_err_stub 10 # ISR 10
isr_err_stub 11 # ISR 11
isr_err_stub 12 # ISR 12
isr_err_stub 13 # ISR 13
isr_err_stub 14 # ISR 14
isr_no_err_stub 15 # ISR 15
isr_no_err_stub 16 # ISR 16
isr_err_stub 17 # ISR 17
isr_no_err_stub 18 # ISR 18
isr_no_err_stub 19 # ISR 19
isr_no_err_stub 20 # ISR 20
isr_err_stub 21 # ISR 21
isr_no_err_stub 22 # Reserved for future use
isr_no_err_stub 23 # Reserved for future use
isr_no_err_stub 24 # Reserved for future use
isr_no_err_stub 25 # Reserved for future use
isr_no_err_stub 26 # Reserved for future use
isr_no_err_stub 27 # Reserved for future use
isr_no_err_stub 28 # ISR 28
isr_err_stub 29 # ISR 29
isr_err_stub 30 # ISR 30
isr_no_err_stub 31 # Reserved for future use