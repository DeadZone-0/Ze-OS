/* keyboard.c - PS/2 Keyboard Driver */

#include "keyboard.h"
#include "idt.h"
#include "io.h"
#include "shell.h"

extern void terminal_writestring(const char *data);
extern void terminal_putchar(char c);

/* Basic US standard keyboard scancode mapping for the lower 128 characters.
 * Non-printable or special keys are mapped to 0 for now. */
static const char
    kbdus[128] =
        {
            0,    27,  '1',  '2', '3',  '4', '5', '6', '7', '8',
            '9',  '0', '-',  '=', '\b', /* Backspace */
            '\t',                       /* Tab */
            'q',  'w', 'e',  'r', 't',  'y', 'u', 'i', 'o', 'p',
            '[',  ']', '\n', /* Enter */
            0,               /* Control */
            'a',  's', 'd',  'f', 'g',  'h', 'j', 'k', 'l', ';',
            '\'', '`', 0, /* Left Shift */
            '\\', 'z', 'x',  'c', 'v',  'b', 'n', 'm', ',', '.',
            '/',  0,                                           /* Right Shift */
            '*',  0,                                           /* Alt */
            ' ',                                               /* Space bar */
            0,                                                 /* Caps lock */
            0,    0,   0,    0,   0,    0,   0,   0,   0,   0, /* F1 - F10 */
            0,                                                 /* Num lock */
            0,                                                 /* Scroll Lock */
            0,                                                 /* Home key */
            0,                                                 /* Up Arrow */
            0,                                                 /* Page Up */
            '-',  0,                                           /* Left Arrow */
            0,    0,                                           /* Right Arrow */
            '+',  0,                                           /* End key */
            0,                                                 /* Down Arrow */
            0,                                                 /* Page Down */
            0,                                                 /* Insert Key */
            0,                                                 /* Delete Key */
            0,    0,   0,    0,                                /* F11 Key */
            0,                                                 /* F12 Key */
            0, /* All other keys are undefined */
};

/* The interrupt handler for IRQ 1 (Keyboard) */
static void keyboard_callback(struct registers *regs) {
  (void)regs;

  /* The keyboard sends a scancode to I/O port 0x60 when a key is pressed or
   * released. */
  uint8_t scancode = inb(0x60);

  /* If the top bit of the byte we read from the keyboard is set, that means a
   * key has been released. */
  if (scancode & 0x80) {
    /* You can handle shift/ctrl/alt key releases here */
  } else {
    /* A key has been pressed down */
    char c = kbdus[scancode];
    if (c != 0) {
      shell_handle_keypress(c);
    }
  }
}

void keyboard_init(void) {
  /* Register our callback to IRQ 1, which is mapped to Interrupt 33 (0x21) */
  register_interrupt_handler(33, keyboard_callback);
}
