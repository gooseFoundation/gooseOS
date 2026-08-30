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

# PLEASE NOTE: The current defualt architecture is x86_64. To change it, please dont edit this Makefile!
# Make sure to pass the needed architecture on the command line!
ARCH := x86_64
BOOTLOADER := limine

ARCH_DIR := arch/$(ARCH)
BOOTLDR_DIR := loaders/$(BOOTLOADER)

KRNL_ELF := goose64.elf
all: $(KRNL_ELF) # NOTE: This has to be set BEFORE EVERY include or target, this is so shit dont break

include $(BOOTLDR_DIR)/ldr.mk # Get the bootloader config file, this is what builds the final image
include $(ARCH_DIR)/arch.mk # Get the architecure config file, this is gonna be used to get the linker compiler and other tools!

# Include ALL the modules
include init/init.mk
include extern/flanterm.mk
include mm/mm.mk
include extern/tinyprintf.mk
include drivers/drivers.mk
include extern/tinyubsan.mk

# Get all the sources and objects
KRNL_ALL_CSRCS = $(INIT_CSRCS) $(ARCH_SRCS) $(FLANTERM_CSRCS) $(MEM_CSRCS) $(DRIVER_CSRCS) $(TINYPRINTF_CSRCS) $(LUBSAN_SRCS) $(LUACPI_SRCS)
KRNL_ALL_ASMS = $(ARCH_ASMS)
KRNL_ALL_OBJS = $(INIT_OBJS) $(ARCH_OBJS) $(FLANTERM_OBJS) $(MEM_OBJS) $(DRIVER_OBJS) $(TINYPRINTF_OBJS) $(LUBSAN_OBJS) $(LUACPI_OBJS)

all: $(KRNL_ELF)

%.o: %.c
	@echo "CC		$<"
	@$(KRNL_CC) $(KRNL_CFLAGS) $< -o $@

%.o: %.S
	@echo "AS		$<"
	@$(KRNL_AS) $(KRNL_ASFLAGS) $< -o $@

$(KRNL_ELF): $(KRNL_ALL_OBJS)
	@echo "LD		$@"
	@$(KRNL_LD) $(KRNL_ALL_OBJS) $(KRNL_LDFLAGS) -o $@

img: $(KRNL_ELF) build_img
clean:
	$(RM) $(KRNL_ELF) $(KRNL_ALL_OBJS)