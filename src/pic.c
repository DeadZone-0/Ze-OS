/* pic.c - 8259 PIC initialization and remapping */

#include "pic.h"
#include "io.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

#define ICW1_ICW4 0x01
#define ICW1_INIT 0x10
#define ICW4_8086 0x01

void pic_send_eoi(unsigned char irq) {
  if (irq >= 8) {
    /* Send EOI to PIC2 (slave) */
    outb(PIC2_COMMAND, 0x20);
  }
  /* Always send EOI to PIC1 (master) */
  outb(PIC1_COMMAND, 0x20);
}

/*
 * Modern PC architecture uses two 8259 PIC chips. The BIOS maps them to
 * interrupts 0-15 (0x00 - 0x0F). But we just used 0x00 - 0x1F for CPU
 * exceptions in the IDT!
 * We must remap the PICs to start from interrupt 32 (0x20) up to 47 (0x2F).
 */
void pic_remap(int offset1, int offset2) {
  unsigned char a1, a2;

  /* Save current PIC masks */
  a1 = inb(PIC1_DATA);
  a2 = inb(PIC2_DATA);

  /* Start initialization sequence in cascade mode */
  outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
  io_wait();
  outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
  io_wait();

  /* Set vector offsets */
  outb(PIC1_DATA, offset1); /* Master PIC vector offset */
  io_wait();
  outb(PIC2_DATA, offset2); /* Slave PIC vector offset */
  io_wait();

  /* Tell Master PIC there is a slave PIC at IRQ2 (0000 0100) */
  outb(PIC1_DATA, 4);
  io_wait();

  /* Tell Slave PIC its cascade identity (0000 0010) */
  outb(PIC2_DATA, 2);
  io_wait();

  /* Tell PICs they are operating in 8086 mode */
  outb(PIC1_DATA, ICW4_8086);
  io_wait();
  outb(PIC2_DATA, ICW4_8086);
  io_wait();

  /* Restore saved masks (or we could zero them to enable all IRQs) */
  outb(PIC1_DATA, a1);
  outb(PIC2_DATA, a2);
}
