/* isr.c - High level interrupt service routines */

#include "idt.h"

/* Convert an integer to a string (base 10 or 16) - used for printing error
 * codes */
static void itoa(uint32_t val, int base, char *buf) {
  static const char digits[] = "0123456789ABCDEF";
  char tmp[32];
  int i = 0;

  if (val == 0) {
    buf[0] = '0';
    buf[1] = '\0';
    return;
  }

  while (val != 0) {
    tmp[i++] = digits[val % base];
    val /= base;
  }

  /* Reverse the string */
  int j = 0;
  while (i > 0) {
    buf[j++] = tmp[--i];
  }
  buf[j] = '\0';
}

/* External terminal output functions from kernel.c */
extern void terminal_writestring(const char *data);
extern void terminal_writestring_color(const char *data, uint8_t fg,
                                       uint8_t bg);

/* Names of the 32 Intel CPU exceptions */
static const char *exception_messages[] = {
    "Divide By Zero Exception",
    "Debug Exception",
    "Non Maskable Interrupt Exception",
    "Breakpoint Exception",
    "Into Detected Overflow Exception",
    "Out of Bounds Exception",
    "Invalid Opcode Exception",
    "No Coprocessor Exception",
    "Double Fault Exception",
    "Coprocessor Segment Overrun Exception",
    "Bad TSS Exception",
    "Segment Not Present Exception",
    "Stack Fault Exception",
    "General Protection Fault Exception",
    "Page Fault Exception",
    "Unknown Interrupt Exception",
    "Coproc. Fault Exception",
    "Alignment Check Exception",
    "Machine Check Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"};

/* This is called by the assembly stub in isr.s */
void isr_handler(struct registers *r) {
  /* Print a red banner denoting a kernel panic / unhandled exception */
  terminal_writestring_color("\n=== SYSTEM EXCEPTION ===\n", 12,
                             0); /* 12 = Light Red */

  if (r->int_no < 32) {
    terminal_writestring_color(exception_messages[r->int_no], 12, 0);
    terminal_writestring("\n");
  } else {
    terminal_writestring_color("Unknown Exception\n", 12, 0);
  }

  char buf[32];
  terminal_writestring("Interrupt #: ");
  itoa(r->int_no, 10, buf);
  terminal_writestring(buf);
  terminal_writestring("\n");

  if (r->err_code != 0) {
    terminal_writestring("Error Code : 0x");
    itoa(r->err_code, 16, buf);
    terminal_writestring(buf);
    terminal_writestring("\n");
  }

  terminal_writestring_color("System Halted.\n", 12, 0);

  /* Disable interrupts and halt forever */
  __asm__ __volatile__("cli; hlt");
  for (;;)
    ;
}
