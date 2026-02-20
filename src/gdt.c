/*
 * gdt.c - Global Descriptor Table setup
 * Defines a flat memory model with null, kernel code, kernel data,
 * user code, and user data segments.
 */

#include "gdt.h"

#define GDT_ENTRIES 5

static struct gdt_entry gdt[GDT_ENTRIES];
static struct gdt_ptr gdtp;

extern void gdt_flush(uint32_t);

static void gdt_set_gate(int idx, uint32_t base, uint32_t limit, uint8_t access,
                         uint8_t gran) {
  gdt[idx].base_low = base & 0xFFFF;
  gdt[idx].base_middle = (base >> 16) & 0xFF;
  gdt[idx].base_high = (base >> 24) & 0xFF;

  gdt[idx].limit_low = limit & 0xFFFF;
  gdt[idx].granularity = ((limit >> 16) & 0x0F) | (gran & 0xF0);

  gdt[idx].access = access;
}

void gdt_init(void) {
  gdtp.limit = (sizeof(struct gdt_entry) * GDT_ENTRIES) - 1;
  gdtp.base = (uint32_t)&gdt;

  /* Null segment */
  gdt_set_gate(0, 0, 0, 0, 0);

  /* Kernel code: base=0, limit=4GB, exec/read, ring 0 */
  gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

  /* Kernel data: base=0, limit=4GB, read/write, ring 0 */
  gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

  /* User code: base=0, limit=4GB, exec/read, ring 3 */
  gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);

  /* User data: base=0, limit=4GB, read/write, ring 3 */
  gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

  gdt_flush((uint32_t)&gdtp);
}
