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

ASM_SRCS  := $(wildcard $(SRC_DIR)/*.s)
C_SRCS    := $(wildcard $(SRC_DIR)/*.c)

ASM_OBJS  := $(patsubst $(SRC_DIR)/%.s, $(BUILD_DIR)/asm/%.o, $(ASM_SRCS))
C_OBJS    := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/c/%.o, $(C_SRCS))

OBJS      := $(ASM_OBJS) $(C_OBJS)
KERNEL    := $(BUILD_DIR)/ze-os.bin
ISO       := ze-os.iso

GRUB_CFG  := $(ISO_DIR)/boot/grub/grub.cfg

.PHONY: all clean verify

all: $(ISO)

$(BUILD_DIR)/asm/%.o: $(SRC_DIR)/%.s | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/c/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL): $(OBJS)
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
