/*
 * kernel.c - Ze-OS kernel
 * VGA text mode driver and kernel entry point.
 */

#include "gdt.h"
#include "idt.h"
#include "pic.h"
#include "timer.h"
#include <stddef.h>
#include <stdint.h>

#define VGA_ADDRESS 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

enum vga_color {
  VGA_COLOR_BLACK = 0,
  VGA_COLOR_BLUE = 1,
  VGA_COLOR_GREEN = 2,
  VGA_COLOR_CYAN = 3,
  VGA_COLOR_RED = 4,
  VGA_COLOR_MAGENTA = 5,
  VGA_COLOR_BROWN = 6,
  VGA_COLOR_LIGHT_GREY = 7,
  VGA_COLOR_DARK_GREY = 8,
  VGA_COLOR_LIGHT_BLUE = 9,
  VGA_COLOR_LIGHT_GREEN = 10,
  VGA_COLOR_LIGHT_CYAN = 11,
  VGA_COLOR_LIGHT_RED = 12,
  VGA_COLOR_LIGHT_MAGENTA = 13,
  VGA_COLOR_LIGHT_BROWN = 14,
  VGA_COLOR_WHITE = 15,
};

static size_t terminal_row;
static size_t terminal_col;
static uint8_t terminal_color;
static uint16_t *terminal_buffer;

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
  return fg | (bg << 4);
}

static inline uint16_t vga_entry(unsigned char c, uint8_t color) {
  return (uint16_t)c | ((uint16_t)color << 8);
}

static size_t strlen(const char *str) {
  size_t len = 0;
  while (str[len])
    len++;
  return len;
}

static void terminal_initialize(void) {
  terminal_row = 0;
  terminal_col = 0;
  terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
  terminal_buffer = (uint16_t *)VGA_ADDRESS;

  for (size_t y = 0; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      const size_t index = y * VGA_WIDTH + x;
      terminal_buffer[index] = vga_entry(' ', terminal_color);
    }
  }
}

static void terminal_setcolor(uint8_t color) { terminal_color = color; }

static void terminal_scroll(void) {
  for (size_t y = 1; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      const size_t dst = (y - 1) * VGA_WIDTH + x;
      const size_t src = y * VGA_WIDTH + x;
      terminal_buffer[dst] = terminal_buffer[src];
    }
  }

  for (size_t x = 0; x < VGA_WIDTH; x++) {
    const size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(' ', terminal_color);
  }
}

static void terminal_putchar(char c) {
  if (c == '\n') {
    terminal_col = 0;
    terminal_row++;
  } else {
    const size_t index = terminal_row * VGA_WIDTH + terminal_col;
    terminal_buffer[index] = vga_entry((unsigned char)c, terminal_color);
    terminal_col++;
  }

  if (terminal_col >= VGA_WIDTH) {
    terminal_col = 0;
    terminal_row++;
  }

  if (terminal_row >= VGA_HEIGHT) {
    terminal_scroll();
    terminal_row = VGA_HEIGHT - 1;
  }
}

void terminal_writestring(const char *data) {
  size_t len = strlen(data);
  for (size_t i = 0; i < len; i++)
    terminal_putchar(data[i]);
}

void terminal_writestring_color(const char *data, enum vga_color fg,
                                enum vga_color bg) {
  uint8_t prev_color = terminal_color;
  terminal_setcolor(vga_entry_color(fg, bg));
  terminal_writestring(data);
  terminal_setcolor(prev_color);
}

void kernel_main(uint32_t multiboot_magic, uint32_t multiboot_info) {
  (void)multiboot_magic;
  (void)multiboot_info;

  terminal_initialize();
  gdt_init();
  idt_init();

  terminal_writestring_color("=========================================="
                             "======================================\n",
                             VGA_COLOR_CYAN, VGA_COLOR_BLACK);

  terminal_writestring_color("                        Welcome to Ze-OS!\n",
                             VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);

  terminal_writestring_color("                      Boot successful.\n",
                             VGA_COLOR_WHITE, VGA_COLOR_BLACK);

  terminal_writestring_color("=========================================="
                             "======================================\n",
                             VGA_COLOR_CYAN, VGA_COLOR_BLACK);

  terminal_writestring("\n");

  terminal_writestring_color("  Architecture : ", VGA_COLOR_LIGHT_CYAN,
                             VGA_COLOR_BLACK);
  terminal_writestring("32-bit x86\n");

  terminal_writestring_color("  Boot Standard: ", VGA_COLOR_LIGHT_CYAN,
                             VGA_COLOR_BLACK);
  terminal_writestring("Multiboot 1 (GRUB)\n");

  terminal_writestring_color("  VGA Mode     : ", VGA_COLOR_LIGHT_CYAN,
                             VGA_COLOR_BLACK);
  terminal_writestring("80x25 Text Mode\n");

  terminal_writestring("\n");

  terminal_writestring_color("  Ze-OS kernel loaded and running.\n",
                             VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);

  terminal_writestring("\nInitializing Hardware Devices...\n");

  /* Remap PIC vectors to 0x20 and 0x28 */
  pic_remap(0x20, 0x28);
  terminal_writestring("  [+] PIC properly remapped.\n");

  /* Initialize PIT timer to 100 Hz */
  timer_init(100);
  terminal_writestring("  [+] PIT initialized (100 Hz).\n");

  terminal_writestring("\nEnabling Hardware Interrupts (STI)... ");
  __asm__ __volatile__("sti");
  terminal_writestring_color("DONE\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);

  terminal_writestring("\nIdling gracefully...\n");
  /* Let the OS idle. The timer interrupt will fire in the background. */
  for (;;) {
    __asm__ __volatile__("hlt");
  }
}
