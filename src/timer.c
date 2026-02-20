/* timer.c - Programmable Interval Timer (PIT) */

#include "timer.h"
#include "idt.h"
#include "io.h"

extern void terminal_writestring(const char *data);

/* Keep track of how many ticks the system has been running */
static uint32_t tick = 0;

/* The callback we register for IRQ0 (Timer) */
static void timer_callback(struct registers *regs) {
  (void)regs;
  tick++;

  /* We can print a dot every 100 ticks (approx 1 second if freq=100Hz)
   * but we'll disable it for now so it doesn't flood the screen. */
  if (tick % 100 == 0) {
    // terminal_writestring(".");
  }
}

uint32_t timer_get_ticks(void) { return tick; }

void timer_init(uint32_t frequency) {
  /* Register our timer callback on IRQ0, which is mapped to Interrupt 32 */
  register_interrupt_handler(32, timer_callback);

  /* The value we send to the PIT is the value to divide its input clock
   * (1193180 Hz) by, to get our required frequency.
   * E.g., for 100Hz, we divide 1193180 by 100 = 11931. */
  uint32_t divisor = 1193180 / frequency;

  /* Command byte to configure the PIT:
   * 0x36 = 0011 0110 in binary
   * Bits 6-7 = 00 (Channel 0)
   * Bits 4-5 = 11 (Access mode: lobyte/hibyte)
   * Bits 1-3 = 011 (Operating mode 3: Square wave generator)
   * Bit  0   = 0 (16-bit binary) */
  outb(0x43, 0x36);

  /* Divisor has to be sent byte-wise, so split into upper/lower bytes. */
  uint8_t l = (uint8_t)(divisor & 0xFF);
  uint8_t h = (uint8_t)((divisor >> 8) & 0xFF);

  /* Send the frequency divisor to Channel 0 data port */
  outb(0x40, l);
  outb(0x40, h);
}
