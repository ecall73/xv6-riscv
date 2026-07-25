.section .data
.balign 4096
.globl _ramdisk_start
_ramdisk_start:
.incbin "fs.img"
.globl _ramdisk_end
_ramdisk_end:
