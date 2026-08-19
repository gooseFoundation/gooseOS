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

BOOTLOADER_NAME = "Limine"
OUTPUT_IMG = build/gooseOS.img

LIMINE_DIR = build/limine

# Clones the Github repo and builds limine into build/limine
# Requires the automake, autoconf and clang packages
$(LIMINE_DIR):
	git clone https://github.com/limine-bootloader/limine.git --depth 1 --branch=trunk $(LIMINE_DIR)
	$(LIMINE_DIR)/bootstrap

	cd $(LIMINE_DIR) && ./configure --enable-all
	gmake -C $(LIMINE_DIR)

	ls $(LIMINE_DIR)/bin

build_img: $(KRNL_ELF) build/limine
	mkdir -p build/img/limine

	dd if=/dev/zero of="$(OUTPUT_IMG)" bs=1M count=2048 status=progress
		
# Partition: GPT, 32mb ESP and the rest is root
	sgdisk --clear "$(OUTPUT_IMG)"
	sgdisk --new 1::0:+32M --typecode 1:EF00 "$(OUTPUT_IMG)"
	sgdisk --new 2::0:0 --typecode 2:0700 "$(OUTPUT_IMG)"

	@$(eval LOOP_DEV := $(shell sudo losetup --find --show -P $(OUTPUT_IMG)))

	sudo mkfs.fat -F 32 "$(LOOP_DEV)p1" -n LIMINE
	sudo mkfs.fat -F 32 "$(LOOP_DEV)p2" -n GOOSEOS

	mkdir -p /tmp/goose-mnt
	sudo mount "$(LOOP_DEV)p1" /tmp/goose-mnt

	sudo mkdir -p /tmp/goose-mnt/EFI/BOOT
	sudo cp "$(LIMINE_DIR)/bin/BOOTX64.EFI" /tmp/goose-mnt/EFI/BOOT/
	sudo cp "$(LIMINE_DIR)/bin/BOOTIA32.EFI" /tmp/goose-mnt/EFI/BOOT/ 2>/dev/null || true
	sudo mkdir -p /tmp/goose-mnt/boot
	sudo cp "$(KRNL_ELF)" /tmp/goose-mnt/boot/kernel.elf

	sudo cp loaders/limine/limine.conf /tmp/goose-mnt/boot/limine.conf

	sudo umount /tmp/goose-mnt

	sudo umount /tmp/goose-mnt 2>/dev/null || true
	sudo losetup -d $(LOOP_DEV)