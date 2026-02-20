/*
 * gdt.h - Global Descriptor Table
 */

#ifndef GDT_H
#define GDT_H

#include <stdint.h>

/* GDT entry (8 bytes each) */
struct gdt_entry {
  uint16_t limit_low;
  uint16_t base_low;
  uint8_t base_middle;
  uint8_t access;
  uint8_t granularity;
  uint8_t base_high;
} __attribute__((packed));

/* GDTR pointer passed to lgdt */
struct gdt_ptr {
  uint16_t limit;
  uint32_t base;
} __attribute__((packed));

void gdt_init(void);

#endif
