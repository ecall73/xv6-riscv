#include "kernel/types.h"
#include "kernel/memlayout.h"
#include "kernel/riscv.h"
#include "kernel/defs.h"

void
platform_map(pagetable_t kpgtbl)
{
  kvmmap(kpgtbl, UART0, UART0, PGSIZE, PTE_R | PTE_W);
  kvmmap(kpgtbl, VIRTIO0, VIRTIO0, PGSIZE, PTE_R | PTE_W);
  kvmmap(kpgtbl, PLIC, PLIC, 0x4000000, PTE_R | PTE_W);
}

void
platform_init(void)
{
  plicinit();
}

void
platform_init_hart(void)
{
  plicinithart();
}

void
platform_timerintr(void)
{
}

int
platform_devintr(void)
{
  int irq = plic_claim();

  if (irq == UART0_IRQ) {
    uartintr();
  } else if (irq == VIRTIO0_IRQ) {
    virtio_disk_intr();
  } else if (irq) {
    printk("unexpected interrupt irq=%d\n", irq);
  }

  if (irq)
    plic_complete(irq);
  return 1;
}
