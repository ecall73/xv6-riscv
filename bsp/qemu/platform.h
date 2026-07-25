#ifndef XV6_BSP_QEMU_PLATFORM_H
#define XV6_BSP_QEMU_PLATFORM_H

#define UART0     0x10000000L
#define UART0_IRQ 10

#define VIRTIO0     0x10001000L
#define VIRTIO0_IRQ 1

#define PLIC                 0x0c000000L
#define PLIC_SENABLE(hart)   (PLIC + 0x2080 + (hart) * 0x100)
#define PLIC_SPRIORITY(hart) (PLIC + 0x201000 + (hart) * 0x2000)
#define PLIC_SCLAIM(hart)    (PLIC + 0x201004 + (hart) * 0x2000)

#define KERNBASE 0x80000000L
#define PHYSTOP  (KERNBASE + 128 * 1024 * 1024)

#ifndef __ASSEMBLER__
void plicinit(void);
void plicinithart(void);
int  plic_claim(void);
void plic_complete(int);
void virtio_disk_intr(void);
#endif

#endif
