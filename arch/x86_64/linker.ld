/* Tell the linker we want x86_64 ELF output */
OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)

/* This defines the base address of the kernel, Limine expects kernels in higher half! */
ENTRY(_start)

SECTIONS {
    /* Start at 2MB in the higher half of memory */
    . = 0xffffffff80200000;

    /* This is the Limine header, its what is used to boot our kernel */
    .limine_requests : {
        KEEP(*(.limine_requests_start))
        KEEP(*(.limine_requests))
        KEEP(*(.limine_requests_end))
    }

    /* Executable code */
    .text : ALIGN(4096) {
        *(.text .text.*)
    }

    /* Read-write data (initialized) */
    .data : ALIGN(4096) {
        *(.data .data.*)
    }

    /* Read-only data */
    .rodata : ALIGN(8) {
        *(.rodata .rodata.*)
    }

    /* Read-write data (uninitialized) */
    .bss : ALIGN(4096) {
        *(.bss .bss.*)
        *(COMMON)
    }
}