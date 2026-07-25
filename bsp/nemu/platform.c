#include "kernel/types.h"
#include "kernel/memlayout.h"
#include "kernel/riscv.h"
#include "kernel/defs.h"

#define KEYDOWN_MASK 0x8000

enum {
  KEY_NONE,
  KEY_ESCAPE, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6,
  KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
  KEY_GRAVE, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7,
  KEY_8, KEY_9, KEY_0, KEY_MINUS, KEY_EQUALS, KEY_BACKSPACE,
  KEY_TAB, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U,
  KEY_I, KEY_O, KEY_P, KEY_LEFTBRACKET, KEY_RIGHTBRACKET, KEY_BACKSLASH,
  KEY_CAPSLOCK, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J,
  KEY_K, KEY_L, KEY_SEMICOLON, KEY_APOSTROPHE, KEY_RETURN,
  KEY_LSHIFT, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M,
  KEY_COMMA, KEY_PERIOD, KEY_SLASH, KEY_RSHIFT,
  KEY_LCTRL, KEY_APPLICATION, KEY_LALT, KEY_SPACE, KEY_RALT, KEY_RCTRL,
  KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_INSERT, KEY_DELETE,
  KEY_HOME, KEY_END, KEY_PAGEUP, KEY_PAGEDOWN,
  KEY_COUNT
};

static int shift_down;
static int ctrl_down;
static int caps_lock;

static int
key_ascii(uint code)
{
  static const char plain[KEY_COUNT] = {
    [KEY_GRAVE] = '`', [KEY_1] = '1', [KEY_2] = '2', [KEY_3] = '3',
    [KEY_4] = '4', [KEY_5] = '5', [KEY_6] = '6', [KEY_7] = '7',
    [KEY_8] = '8', [KEY_9] = '9', [KEY_0] = '0', [KEY_MINUS] = '-',
    [KEY_EQUALS] = '=', [KEY_BACKSPACE] = '\b', [KEY_TAB] = '\t',
    [KEY_Q] = 'q', [KEY_W] = 'w', [KEY_E] = 'e', [KEY_R] = 'r',
    [KEY_T] = 't', [KEY_Y] = 'y', [KEY_U] = 'u', [KEY_I] = 'i',
    [KEY_O] = 'o', [KEY_P] = 'p', [KEY_LEFTBRACKET] = '[',
    [KEY_RIGHTBRACKET] = ']', [KEY_BACKSLASH] = '\\', [KEY_A] = 'a',
    [KEY_S] = 's', [KEY_D] = 'd', [KEY_F] = 'f', [KEY_G] = 'g',
    [KEY_H] = 'h', [KEY_J] = 'j', [KEY_K] = 'k', [KEY_L] = 'l',
    [KEY_SEMICOLON] = ';', [KEY_APOSTROPHE] = '\'', [KEY_RETURN] = '\n',
    [KEY_Z] = 'z', [KEY_X] = 'x', [KEY_C] = 'c', [KEY_V] = 'v',
    [KEY_B] = 'b', [KEY_N] = 'n', [KEY_M] = 'm', [KEY_COMMA] = ',',
    [KEY_PERIOD] = '.', [KEY_SLASH] = '/', [KEY_SPACE] = ' ',
  };
  static const char shifted[KEY_COUNT] = {
    [KEY_GRAVE] = '~', [KEY_1] = '!', [KEY_2] = '@', [KEY_3] = '#',
    [KEY_4] = '$', [KEY_5] = '%', [KEY_6] = '^', [KEY_7] = '&',
    [KEY_8] = '*', [KEY_9] = '(', [KEY_0] = ')', [KEY_MINUS] = '_',
    [KEY_EQUALS] = '+', [KEY_LEFTBRACKET] = '{', [KEY_RIGHTBRACKET] = '}',
    [KEY_BACKSLASH] = '|', [KEY_SEMICOLON] = ':', [KEY_APOSTROPHE] = '"',
    [KEY_COMMA] = '<', [KEY_PERIOD] = '>', [KEY_SLASH] = '?',
  };

  if (code >= KEY_COUNT)
    return 0;
  int c = plain[code];
  if (c >= 'a' && c <= 'z') {
    if (shift_down ^ caps_lock)
      c -= 'a' - 'A';
    if (ctrl_down)
      c = (c | 0x20) - 'a' + 1;
  } else if (shift_down && shifted[code] != 0) {
    c = shifted[code];
  }
  return c;
}

static void
keyboard_poll(void)
{
  for (int i = 0; i < 16; i++) {
    uint data = *(volatile uint32 *)KBD0;
    uint code = data & ~KEYDOWN_MASK;
    int down = (data & KEYDOWN_MASK) != 0;
    if (code == KEY_NONE)
      return;

    if (code == KEY_LSHIFT || code == KEY_RSHIFT) {
      shift_down = down;
      continue;
    }
    if (code == KEY_LCTRL || code == KEY_RCTRL) {
      ctrl_down = down;
      continue;
    }
    if (code == KEY_CAPSLOCK && down) {
      caps_lock = !caps_lock;
      continue;
    }
    if (!down)
      continue;

    const char *seq = 0;
    if (code == KEY_UP)
      seq = "\033[A";
    else if (code == KEY_DOWN)
      seq = "\033[B";
    else if (code == KEY_RIGHT)
      seq = "\033[C";
    else if (code == KEY_LEFT)
      seq = "\033[D";

    if (seq != 0) {
      while (*seq != 0)
        consoleintr(*seq++);
    } else {
      int c = key_ascii(code);
      if (c != 0)
        consoleintr(c);
    }
  }
}

void
platform_map(pagetable_t kpgtbl)
{
  uint32 base = PGROUNDDOWN(UART0);
  kvmmap(kpgtbl, base, base, PGSIZE, PTE_R | PTE_W);
}

void
platform_init(void)
{
}

void
platform_init_hart(void)
{
}

void
platform_timerintr(void)
{
  keyboard_poll();
}

int
platform_devintr(void)
{
  return 0;
}
