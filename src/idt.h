/* idt.h - Interrupt Descriptor Table */

#ifndef IDT_H
#define IDT_H

#include <stdint.h>

/* IDT entry (8 bytes each) */
struct idt_entry {
  uint16_t base_low;
  uint16_t sel;
  uint8_t always0;
  uint8_t flags;
  uint16_t base_high;
} __attribute__((packed));

/* IDTR pointer passed to lidt */
struct idt_ptr {
  uint16_t limit;
  uint32_t base;
} __attribute__((packed));

/* Registers state passed to C interrupt handlers */
struct registers {
  uint32_t ds;
  uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
  uint32_t int_no, err_code;
  uint32_t eip, cs, eflags, useresp, ss;
};

void idt_init(void);
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);

#endif
