# *
# * ===========================================
# * Copyright(c) 2026-present IgosProjects
# * Copyright(c) 2026-present TheGooseFoundation and contributors
# * 
# * GooseOS is licensed under the GNU General Public License version 3(GPLv3), 
# * the license text can be found inside of the LICENSE file at the root of the repositoy
# *
# * If you didnt receive the LICENSE file with this source code, 
# * it can be found at https://www.gnu.org/licenses/gpl-3.0.html
# * ==========================================
# *

# This is the architecture Makefile for the x86_64 architecture
# It holds the compiler, linker and assembler that are used for compiling the kernel!

Ax86_64PREFIX = x86_64-elf

# Compiler, Linker and Assembler(we use GCC instead of AS so .S files are preprocessed automaticly)
KRNL_CC := $(Ax86_64PREFIX)-gcc
KRNL_LD := $(Ax86_64PREFIX)-ld
KRNL_AS := $(Ax86_64PREFIX)-gcc

# Flags for the compiler, linker and assembler
KRNL_CFLAGS := -c -ffreestanding -nostdlib -I include -mcmodel=large -mno-red-zone -g -O0 -I arch/x86_64/include
KRNL_LDFLAGS := -T arch/x86_64/linker.ld -g
KRNL_ASFLAGS := -c -g

ARCH_ASMS := arch/x86_64/boot/boot.S arch/x86_64/boot/gdt.S arch/x86_64/cpu/hcf.S arch/x86_64/cpu/int.S
ARCH_SRCS := arch/x86_64/boot/headers/limine.c arch/x86_64/boot/entry.c arch/x86_64/cpu/idt.c arch/x86_64/cpu/ints.c arch/x86_64/cpu/cpu.c arch/x86_64/mm/pmm.c arch/x86_64/mm/vmm.c

ARCH_OBJS = arch/x86_64/boot/boot.o arch/x86_64/cpu/hcf.o arch/x86_64/boot/headers/limine.o arch/x86_64/boot/entry.o arch/x86_64/cpu/idt.o arch/x86_64/cpu/ints.o arch/x86_64/cpu/int.o arch/x86_64/cpu/cpu.o arch/x86_64/boot/gdt.o arch/x86_64/mm/pmm.o arch/x86_64/io/ports.o arch/x86_64/mm/vmm.o