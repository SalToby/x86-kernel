# ==============================
# Toolchain
# ==============================
CC := i686-elf-gcc
AS := nasm
LD := i686-elf-gcc

# ==============================
# Flags
# ==============================
CFLAGS := -ffreestanding -O0 -g -Wall -Wextra -std=gnu99 \
          -fno-stack-protector -fno-pic -fno-builtin \
          -Iinclude

ASFLAGS := -f elf32
LDFLAGS := -T boot/linker.ld -nostdlib -ffreestanding
LIBS    := -lgcc

# ==============================
# Directories
# ==============================
BUILD   := build
ISO_DIR := iso
BOOTDIR := $(ISO_DIR)/boot
KERNEL  := $(BOOTDIR)/os.elf
ISO_IMG := iso/os.iso

# ==============================
# Source discovery
# ==============================
K_C_SRCS   := $(shell find kernel -name "*.c")
K_ASM_SRCS := $(shell find kernel -name "*.asm")
BOOT_ASM   := $(shell find boot -name "*.asm")

# ==============================
# Object files
# ==============================
K_C_OBJS   := $(patsubst %.c,$(BUILD)/%.o,$(K_C_SRCS))
K_ASM_OBJS := $(patsubst %.asm,$(BUILD)/%.o,$(K_ASM_SRCS))
BOOT_OBJS  := $(patsubst %.asm,$(BUILD)/%.o,$(BOOT_ASM))

OBJS := $(K_C_OBJS) $(K_ASM_OBJS) $(BOOT_OBJS)

# ==============================
# Targets
# ==============================
all: $(ISO_IMG)

# ==============================
# Compile C
# ==============================
$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# ==============================
# Compile ASM (NASM)
# ==============================
$(BUILD)/%.o: %.asm
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

# ==============================
# Link kernel
# ==============================
$(KERNEL): $(OBJS)
	@mkdir -p $(BOOTDIR)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

# ==============================
# Build ISO
# ==============================
$(ISO_IMG): $(KERNEL)
	grub-mkrescue -o $(ISO_IMG) $(ISO_DIR)

# ==============================
# Run
# ==============================
run: $(ISO_IMG)
	qemu-system-i386 -cpu qemu32 -cdrom $(ISO_IMG)

run-debug: $(ISO_IMG)
	qemu-system-i386 -cpu qemu32 -cdrom $(ISO_IMG) -d int -D qemu.log

run-gdb: $(ISO_IMG)
	qemu-system-i386 -cpu qemu32 -cdrom $(ISO_IMG) -S -s
# ==============================
# Clean
# ==============================
clean:
	rm -rf build
	rm -f $(ISO_IMG)
	rm -rf iso/boot/os.elf
.PHONY: all run clean
