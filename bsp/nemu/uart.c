#include "kernel/types.h"
#include "kernel/memlayout.h"
#include "kernel/riscv.h"
#include "kernel/defs.h"

void
uartinit(void)
{
}

void
uartwrite(char buf[], int n)
{
  for (int i = 0; i < n; i++)
    uartputc_sync(buf[i]);
}

void
uartputc_sync(int c)
{
  *(volatile uchar *)UART0 = (uchar)c;
}

void
uartintr(void)
{
}
