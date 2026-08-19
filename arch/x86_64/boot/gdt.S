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

.section .data
.global _load_gdt
.code64

# This is the GDT, a table required by the x86 architecture for operating systems
# It's a leftover from the old days of segmentation but it still hasn't been removed and is now used as a permission slip
gdt:
    gdt_null:
        .quad 0x0000000000000000
    gdt_code:
        .quad 0x00AF9A000000FFFF
    gdt_data:
        .quad 0x00CF92000000FFFF
gdt_end:

# GDT pointer structure, actual thing passed to "lgdt"
gdt_ptr:
    .word gdt_end - gdt 
    .quad gdt

# Loads the gooseOS GDT and switches segments to the correct ones
_load_gdt:
    lgdt [gdt_ptr] # Load the actual GDT(i spent HALF AN HOUR DEBUGGING THIS)

    # Set the Boring Segments(new offical name for them)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax

    # Set the Code Segment, it wants more attention for some reason
    # Should we give it a dog treat?
    pop rsi # Pop the return address
    mov rax, rsp # Store the original stack

    push 0x10 # Push SS(Stack Segment)
    push rax # Push stack pointer
    pushfq # Push RFLAGS
    push 0x08 # Push CS(Code Segment)
    push rsi # Push the return address back to the stack
    iretq # Cause the CPU to return using the structure in memory