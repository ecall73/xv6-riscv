OUTPUT_ARCH("riscv")
ENTRY(_start)

PHDRS
{
  entry   PT_LOAD FLAGS(5);
  ssbl    PT_LOAD FLAGS(5);
  runtime PT_LOAD FLAGS(7);
  bss     PT_LOAD FLAGS(6);
}

MEMORY
{
  flash (rx)  : ORIGIN = 0x30000000, LENGTH = 0x01000000
  sram  (rwx) : ORIGIN = 0x0f000000, LENGTH = 0x00002000
  sdram (rwx) : ORIGIN = 0xa0000000, LENGTH = 0x02000000
}

SECTIONS
{
  .entry ORIGIN(flash) : {
    KEEP(*(.entry))
  } > flash : entry

  .ssbl ORIGIN(sram) : AT(ALIGN(LOADADDR(.entry) + SIZEOF(.entry), 4)) {
    _ssbl_vma_start = .;
    KEEP(*(.ssbl))
    . = ALIGN(4);
    _ssbl_vma_end = .;
  } > sram : ssbl
  _ssbl_lma_start = LOADADDR(.ssbl);
  _ssbl_lma_end = LOADADDR(.ssbl) + SIZEOF(.ssbl);

  .runtime ORIGIN(sdram) : AT(ALIGN(LOADADDR(.ssbl) + SIZEOF(.ssbl), 4)) {
    _runtime_vma_start = .;
    kernel/entry.o(.text .text.*)
    *(.text .text.*)
    . = ALIGN(0x1000);
    _trampoline = .;
    *(trampsec)
    . = ALIGN(0x1000);
    ASSERT(. - _trampoline == 0x1000,
           "error: trampoline larger than one page");
    PROVIDE(etext = .);

    . = ALIGN(16);
    *(.srodata .srodata.*)
    *(.rodata .rodata.*)
    *(.eh_frame .eh_frame.*)
    . = ALIGN(16);
    *(.sdata .sdata.*)
    *(.data .data.*)
    *(.got .got.*)
    . = ALIGN(16);
    _runtime_vma_end = .;
  } > sdram : runtime
  _runtime_lma_start = LOADADDR(.runtime);
  _runtime_lma_end = LOADADDR(.runtime) + SIZEOF(.runtime);

  .bss (NOLOAD) : {
    _bss_start = .;
    *(.sbss .sbss.*)
    *(.bss .bss.*)
    *(COMMON)
    . = ALIGN(16);
    _bss_end = .;
  } > sdram : bss

  PROVIDE(end = .);

  ASSERT(_runtime_lma_end <= ORIGIN(flash) + LENGTH(flash),
         "Flash overflow: xv6 image exceeds ysyxSoC flash")
  ASSERT(_ssbl_vma_end <= ORIGIN(sram) + LENGTH(sram),
         "SRAM overflow: xv6 SSBL exceeds ysyxSoC SRAM")
  ASSERT(end <= ORIGIN(sdram) + LENGTH(sdram),
         "SDRAM overflow: xv6 kernel image exceeds ysyxSoC SDRAM")
}
