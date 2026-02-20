# Ze-OS Makefile

AS      := nasm
CC      := gcc
LD      := ld

ASFLAGS := -f elf32
CFLAGS  := -m32 -ffreestanding -O2 -Wall -Wextra -std=gnu99
LDFLAGS := -m elf_i386 -T src/linker.ld -nostdlib

SRC_DIR   := src
BUILD_DIR := build
ISO_DIR   := iso

ASM_SRC   := $(SRC_DIR)/boot.s
C_SRC     := $(SRC_DIR)/kernel.c

ASM_OBJ   := $(BUILD_DIR)/boot.o
C_OBJ     := $(BUILD_DIR)/kernel.o

KERNEL    := $(BUILD_DIR)/ze-os.bin
ISO       := ze-os.iso

GRUB_CFG  := $(ISO_DIR)/boot/grub/grub.cfg

.PHONY: all clean verify

all: $(ISO)

$(ASM_OBJ): $(ASM_SRC) | $(BUILD_DIR)
	$(AS) $(ASFLAGS) $< -o $@

$(C_OBJ): $(C_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL): $(ASM_OBJ) $(C_OBJ)
	$(LD) $(LDFLAGS) -o $@ $^

verify: $(KERNEL)
	@if grub-file --is-x86-multiboot $(KERNEL); then \
		echo "$(KERNEL) is a valid Multiboot kernel"; \
	else \
		echo "$(KERNEL) is NOT a valid Multiboot kernel"; \
		exit 1; \
	fi

$(GRUB_CFG): | $(ISO_DIR)/boot/grub
	@echo 'menuentry "Ze-OS" {'                  > $@
	@echo '    multiboot /boot/ze-os.bin'        >> $@
	@echo '    boot'                             >> $@
	@echo '}'                                    >> $@

$(ISO): $(KERNEL) $(GRUB_CFG) | verify
	cp $(KERNEL) $(ISO_DIR)/boot/ze-os.bin
	grub-mkrescue -o $@ $(ISO_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(ISO_DIR)/boot/grub:
	mkdir -p $(ISO_DIR)/boot/grub

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(ISO_DIR)/boot
	rm -f $(ISO)
