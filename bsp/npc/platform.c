#include "kernel/types.h"
#include "kernel/memlayout.h"
#include "kernel/riscv.h"
#include "kernel/defs.h"

void
platform_map(pagetable_t kpgtbl)
{
  kvmmap(kpgtbl, UART0, UART0, PGSIZE, PTE_R | PTE_W);
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
}

int
platform_devintr(void)
{
  return 0;
}
