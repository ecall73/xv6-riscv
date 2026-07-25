#include "kernel/types.h"
#include "kernel/param.h"
#include "kernel/spinlock.h"
#include "kernel/sleeplock.h"
#include "kernel/fs.h"
#include "kernel/buf.h"
#include "kernel/riscv.h"
#include "kernel/defs.h"

extern uchar _ramdisk_start[], _ramdisk_end[];

void
disk_init(void)
{
  if (_ramdisk_end - _ramdisk_start < FSSIZE * BSIZE)
    panic("ramdisk too small");
}

void
disk_rw(struct buf *b, int write)
{
  if (b->blockno >= FSSIZE)
    panic("ramdisk block");

  uchar *block = _ramdisk_start + b->blockno * BSIZE;
  if (write)
    memmove(block, b->data, BSIZE);
  else
    memmove(b->data, block, BSIZE);
}
