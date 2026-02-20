/* shell.c - Basic command line shell for Ze-OS */

#include "shell.h"
#include <stddef.h>
#include <stdint.h>


extern void terminal_writestring(const char *data);
extern void terminal_writestring_color(const char *data, uint8_t fg,
                                       uint8_t bg);
extern void terminal_putchar(char c);
extern void terminal_clear(void);

#define CMD_PROMPT "root@ze-os# "
#define BUFFER_SIZE 256

static char buf[BUFFER_SIZE];
static size_t buf_len = 0;

/* Basic string comparison since we don't have libc's strcmp */
static int strcmp(const char *s1, const char *s2) {
  while (*s1 && (*s1 == *s2)) {
    s1++;
    s2++;
  }
  return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

static void shell_prompt(void) {
  terminal_writestring_color(CMD_PROMPT, 10, 0); /* 10 is Light Green */
}

static void execute_command(void) {
  if (buf_len == 0) {
    return;
  }

  if (strcmp(buf, "help") == 0) {
    terminal_writestring("Available commands:\n");
    terminal_writestring("  help  - Show this help message\n");
    terminal_writestring("  clear - Clear the screen\n");
    terminal_writestring("  echo  - Repeat input (e.g., 'echo hello')\n");
  } else if (strcmp(buf, "clear") == 0) {
    terminal_clear();
  } else if (buf[0] == 'e' && buf[1] == 'c' && buf[2] == 'h' && buf[3] == 'o' &&
             buf[4] == ' ') {
    terminal_writestring(&buf[5]);
    terminal_writestring("\n");
  } else {
    terminal_writestring("Command not found: ");
    terminal_writestring(buf);
    terminal_writestring("\n");
  }
}

void shell_init(void) {
  terminal_writestring("\n");
  shell_prompt();
  buf_len = 0;
}

void shell_handle_keypress(char c) {
  if (c == '\n') {
    terminal_putchar('\n');

    /* Null terminate the buffer and execute it */
    buf[buf_len] = '\0';
    execute_command();

    /* Reset buffer for next command */
    buf_len = 0;
    shell_prompt();
  } else if (c == '\b') {
    if (buf_len > 0) {
      terminal_putchar('\b');
      buf_len--;
    }
  } else if (c >= 32 && c <= 126) {
    /* Only accept printable characters to prevent buffer issues */
    if (buf_len < BUFFER_SIZE - 1) {
      buf[buf_len++] = c;
      terminal_putchar(c);
    }
  }
}
