# Ze-OS — VirtualBox Testing Guide

## Prerequisites

1. **Oracle VirtualBox** installed on Windows 11 — [download here](https://www.virtualbox.org/wiki/Downloads)
2. **`ze-os.iso`** built successfully (run `make` in WSL at `/mnt/z/Ze-OS`)

---

## Step 1: Build the ISO (if you haven't already)

Open **WSL Ubuntu** and run:

```bash
cd /mnt/z/Ze-OS
make
```

You should see output ending with `grub-mkrescue` creating `ze-os.iso`. Verify it exists:

```bash
ls -lh ze-os.iso
```

The ISO will be accessible from Windows at `Z:\Ze-OS\ze-os.iso`.

---

## Step 2: Create a New Virtual Machine

1. Open **Oracle VirtualBox** on Windows
2. Click **Machine → New** (or press `Ctrl+N`)
3. Fill in the settings:

| Setting | Value |
|---------|-------|
| **Name** | `Ze-OS` |
| **Folder** | (leave default or choose your preference) |
| **Type** | `Other` |
| **Subtype** | `Other/Unknown` |
| **Version** | `Other/Unknown (32-bit)` |

4. Click **Next**

---

## Step 3: Configure Hardware

### Memory (RAM)
- Set **Base Memory** to **64 MB** (more than enough for our text-mode kernel)
- You can increase this later as Ze-OS grows

### Processors
- **1 CPU** is sufficient

Click **Next**.

---

## Step 4: Virtual Hard Disk

- Select **"Do Not Add a Virtual Hard Disk"**
  - Ze-OS runs entirely from the ISO (no disk needed yet)
  - We'll add a virtual disk in a future phase when we implement a filesystem

Click **Next**, then **Finish**.

---

## Step 5: Configure the VM Settings

Right-click the `Ze-OS` VM → **Settings**, then adjust:

### System → Motherboard
| Setting | Value |
|---------|-------|
| **Boot Order** | ☑ Optical, uncheck everything else |
| **Chipset** | `PIIX3` |
| **Pointing Device** | `PS/2 Mouse` |
| **Extended Features** | ☑ Enable I/O APIC |

> [!IMPORTANT]
> Make sure **"Enable EFI"** is **UNCHECKED**. Ze-OS uses BIOS/Legacy boot via GRUB.

### Display → Screen
| Setting | Value |
|---------|-------|
| **Video Memory** | `16 MB` |
| **Graphics Controller** | `VBoxVGA` |

Click **OK** to save.

---

## Step 6: Attach the ISO

1. Right-click the `Ze-OS` VM → **Settings** → **Storage**
2. Under **Controller: IDE**, click the 💿 **Empty** disk icon
3. On the right side, click the small **disk icon** next to "Optical Drive"
4. Select **"Choose a disk file..."**
5. Navigate to `Z:\Ze-OS\ze-os.iso` and select it
6. Click **OK**

---

## Step 7: Boot Ze-OS!

1. Select the `Ze-OS` VM and click **Start** (or press `Enter`)
2. GRUB will load and show a menu with **"Ze-OS"** as an entry
3. Press `Enter` (or wait — it auto-boots the first entry)
4. You should see the Ze-OS welcome banner:

```
================================================================================
                        Welcome to Ze-OS!
                      Boot successful.
================================================================================

  Architecture : 32-bit x86
  Boot Standard: Multiboot 1 (GRUB)
  VGA Mode     : 80x25 Text Mode

  Ze-OS kernel loaded and running.
```

🎉 **Congratulations — Ze-OS is running!**

---

## Troubleshooting

### "No bootable medium found"
- Verify the ISO is attached in **Storage → Controller: IDE**
- Make sure **Optical** is first in the boot order

### GRUB shows but kernel doesn't load
- Rebuild with `make clean && make` in WSL
- Run `make verify` to check Multiboot compliance

### Black screen / hangs
- Ensure **EFI is disabled** (System → Motherboard)
- Ensure **Version** is set to `Other/Unknown (32-bit)`, not 64-bit
- Try setting Graphics Controller to `VBoxVGA` (not VBoxSVGA)

### Build errors in WSL
- Verify all dependencies: `gcc --version && nasm --version && grub-mkrescue --version`
- If `gcc-multilib` is missing, 32-bit compilation will fail

---

## Re-testing After Code Changes

After modifying any source file, just rebuild and reboot:

```bash
cd /mnt/z/Ze-OS
make clean && make
```

Then in VirtualBox:
1. If the VM is running, close it (**Machine → Power Off**)
2. The ISO is already attached — just click **Start** again
