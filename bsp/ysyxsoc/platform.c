#include "kernel/types.h"
#include "kernel/memlayout.h"
#include "kernel/riscv.h"
#include "kernel/defs.h"

static int ps2_e0;
static int ps2_break;
static int ps2_shift;
static int ps2_ctrl;
static int ps2_caps;

static void
map_device(pagetable_t kpgtbl, uint32 base, uint32 size)
{
  kvmmap(kpgtbl, base, base, size, PTE_R | PTE_W);
}

void
platform_map(pagetable_t kpgtbl)
{
  map_device(kpgtbl, CLINT0, 0x10000);
  map_device(kpgtbl, UART0, PGSIZE);
  map_device(kpgtbl, SPI0, PGSIZE);
  map_device(kpgtbl, GPIO0, PGSIZE);
  map_device(kpgtbl, PS2_0, PGSIZE);
  map_device(kpgtbl, VGA0, VGA0_SIZE);
}

static int
ps2_ascii(uchar code)
{
  static const char plain[128] = {
    [0x0d] = '\t', [0x0e] = '`', [0x15] = 'q', [0x16] = '1',
    [0x1a] = 'z', [0x1b] = 's', [0x1c] = 'a', [0x1d] = 'w',
    [0x1e] = '2', [0x21] = 'c', [0x22] = 'x', [0x23] = 'd',
    [0x24] = 'e', [0x25] = '4', [0x26] = '3', [0x29] = ' ',
    [0x2a] = 'v', [0x2b] = 'f', [0x2c] = 't', [0x2d] = 'r',
    [0x2e] = '5', [0x31] = 'n', [0x32] = 'b', [0x33] = 'h',
    [0x34] = 'g', [0x35] = 'y', [0x36] = '6', [0x3a] = 'm',
    [0x3b] = 'j', [0x3c] = 'u', [0x3d] = '7', [0x3e] = '8',
    [0x41] = ',', [0x42] = 'k', [0x43] = 'i', [0x44] = 'o',
    [0x45] = '0', [0x46] = '9', [0x49] = '.', [0x4a] = '/',
    [0x4b] = 'l', [0x4c] = ';', [0x4d] = 'p', [0x4e] = '-',
    [0x52] = '\'', [0x54] = '[', [0x55] = '=', [0x5a] = '\n',
    [0x5b] = ']', [0x5d] = '\\', [0x66] = '\b',
  };
  static const char shifted[128] = {
    [0x0e] = '~', [0x16] = '!', [0x1e] = '@', [0x25] = '$',
    [0x26] = '#', [0x2e] = '%', [0x36] = '^', [0x3d] = '&',
    [0x3e] = '*', [0x41] = '<', [0x45] = ')', [0x46] = '(',
    [0x49] = '>', [0x4a] = '?', [0x4c] = ':', [0x4e] = '_',
    [0x52] = '"', [0x54] = '{', [0x55] = '+', [0x5b] = '}',
    [0x5d] = '|',
  };

  if (code >= 128)
    return 0;

  int c = plain[code];
  if (c >= 'a' && c <= 'z') {
    if (ps2_shift ^ ps2_caps)
      c -= 'a' - 'A';
    if (ps2_ctrl)
      c = (c | 0x20) - 'a' + 1;
  } else if (ps2_shift && shifted[code] != 0) {
    c = shifted[code];
  }
  return c;
}

static void
ps2_emit_extended(uchar code)
{
  const char *seq = 0;
  if (code == 0x75)
    seq = "\033[A";
  else if (code == 0x72)
    seq = "\033[B";
  else if (code == 0x74)
    seq = "\033[C";
  else if (code == 0x6b)
    seq = "\033[D";

  if (seq != 0)
    while (*seq != 0)
      consoleintr(*seq++);
}

static void
keyboard_poll(void)
{
  for (int i = 0; i < 16; i++) {
    uchar code = *(volatile uint32 *)PS2_0;
    if (code == 0)
      return;
    if (code == 0xe0) {
      ps2_e0 = 1;
      continue;
    }
    if (code == 0xf0) {
      ps2_break = 1;
      continue;
    }

    int released = ps2_break;
    ps2_break = 0;
    if (!ps2_e0 && (code == 0x12 || code == 0x59)) {
      ps2_shift = !released;
      continue;
    }
    if (code == 0x14) {
      ps2_ctrl = !released;
      ps2_e0 = 0;
      continue;
    }
    if (!ps2_e0 && code == 0x58 && !released) {
      ps2_caps = !ps2_caps;
      continue;
    }
    if (released) {
      ps2_e0 = 0;
      continue;
    }
    if (ps2_e0)
      ps2_emit_extended(code);
    else {
      int c = ps2_ascii(code);
      if (c != 0)
        consoleintr(c);
    }
    ps2_e0 = 0;
  }
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
  uartintr();
  keyboard_poll();
}

int
platform_devintr(void)
{
  return 0;
}
