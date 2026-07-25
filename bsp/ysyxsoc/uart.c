#include "kernel/types.h"
#include "kernel/memlayout.h"
#include "kernel/riscv.h"
#include "kernel/defs.h"

#define Reg(reg) ((volatile uchar *)(UART0 + (reg)))
#define ReadReg(reg) (*(Reg(reg)))
#define WriteReg(reg, value) (*(Reg(reg)) = (value))

enum {
  RHR = 0,
  THR = 0,
  IER = 1,
  FCR = 2,
  LCR = 3,
  LSR = 5,
  LCR_DLAB = 1 << 7,
  LCR_8N1 = 3,
  LSR_RX_READY = 1 << 0,
  LSR_TX_IDLE = 1 << 5,
};

void
uartinit(void)
{
  WriteReg(IER, 0);
  WriteReg(LCR, LCR_DLAB);
  WriteReg(0, 1);
  WriteReg(1, 0);
  WriteReg(LCR, LCR_8N1);
  WriteReg(FCR, 0x07);
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
  while ((ReadReg(LSR) & LSR_TX_IDLE) == 0)
    ;
  WriteReg(THR, (uchar)c);
}

void
uartintr(void)
{
  while (ReadReg(LSR) & LSR_RX_READY)
    consoleintr(ReadReg(RHR));
}
