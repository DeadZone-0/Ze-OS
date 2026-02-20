# Ze-OS

A bare-metal 32-bit x86 operating system built from scratch using C and Assembly.

## Features

- Multiboot 1 compliant (GRUB bootloader)
- VGA text mode driver (80x25, 16 colors)
- Screen scrolling, line wrapping, colored output
- Builds into a bootable ISO

## Project Structure

```
Ze-OS/
├── src/
│   ├── boot.s       # Multiboot entry point (NASM)
│   ├── kernel.c     # VGA kernel
│   └── linker.ld    # Linker script (kernel at 1MB)
├── Makefile          # Build system
└── ze-os.iso         # Output (generated)
```

## Building

Requires a Linux environment (WSL Ubuntu recommended):

```bash
sudo apt install build-essential gcc-multilib nasm xorriso grub-pc-bin grub-common mtools
```

Then build the ISO:

```bash
make
```

Clean build artifacts:

```bash
make clean
```

## Running

Boot `ze-os.iso` in any x86 virtual machine (VirtualBox, QEMU, etc.):

**VirtualBox:** Create a VM with Type `Other`, Version `Other/Unknown (32-bit)`, attach the ISO to the optical drive, and boot. Make sure EFI is disabled.

**QEMU:**

```bash
qemu-system-i386 -cdrom ze-os.iso
```

## Architecture

- **Architecture:** 32-bit x86
- **Boot Standard:** Multiboot 1
- **Languages:** C, x86 Assembly (NASM)
- **Output:** Bootable ISO via `grub-mkrescue`

## License

MIT
