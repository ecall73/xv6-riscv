.section .entry, "ax"
.option nopic
.globl _start
.type _start, @function

_start:
  mv s0, zero

  /* FSBL: copy the second-stage loader from flash to SRAM. */
  la t0, _ssbl_lma_start
  la t1, _ssbl_vma_start
  la t2, _ssbl_vma_end
  sub a1, t2, t1
  andi a1, a1, -4
  add a2, t1, a1
1:
  bgeu t1, a2, 2f
  lw a0, 0(t0)
  sw a0, 0(t1)
  addi t0, t0, 4
  addi t1, t1, 4
  j 1b
2:
  bgeu t1, t2, 3f
  lbu a0, 0(t0)
  sb a0, 0(t1)
  addi t0, t0, 1
  addi t1, t1, 1
  j 2b
3:
  fence.i
  la t0, _ssbl_vma_start
  jr t0

.size _start, . - _start

.section .ssbl, "ax"
.globl _ssbl_entry
.type _ssbl_entry, @function

_ssbl_entry:
  /* SSBL: relocate the linked kernel and ramdisk from flash to SDRAM. */
  la t0, _runtime_lma_start
  la t1, _runtime_vma_start
  la t2, _runtime_vma_end
  sub a1, t2, t1
  andi a1, a1, -4
  add a2, t1, a1
4:
  bgeu t1, a2, 5f
  lw a0, 0(t0)
  sw a0, 0(t1)
  addi t0, t0, 4
  addi t1, t1, 4
  j 4b
5:
  bgeu t1, t2, 6f
  lbu a0, 0(t0)
  sb a0, 0(t1)
  addi t0, t0, 1
  addi t1, t1, 1
  j 5b

  /* NOBITS data is absent from the raw image and must be cleared. */
6:
  la t1, _bss_start
  la t2, _bss_end
  sub a1, t2, t1
  andi a1, a1, -4
  add a2, t1, a1
7:
  bgeu t1, a2, 8f
  sw zero, 0(t1)
  addi t1, t1, 4
  j 7b
8:
  bgeu t1, t2, 9f
  sb zero, 0(t1)
  addi t1, t1, 1
  j 8b
9:
  fence.i
  la t0, _entry
  jr t0

.size _ssbl_entry, . - _ssbl_entry
