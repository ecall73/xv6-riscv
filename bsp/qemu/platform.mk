ifneq ($(ARCH),riscv32-qemu)
  $(error bsp/qemu supports ARCH=riscv32-qemu, not $(ARCH))
endif

ARCH_CFLAGS := -march=rv32imac_zicsr_zifencei -mabi=ilp32
BSP_OBJS := \
  bsp/qemu/uart.o \
  bsp/qemu/plic.o \
  bsp/qemu/virtio_disk.o \
  bsp/qemu/platform.o

QEMU ?= qemu-system-riscv32
MIN_QEMU_VERSION := 7.2
CPUS ?= 3

QEMU_VERSION := $(shell $(QEMU) --version | head -n 1 | sed -E 's/^QEMU emulator version ([0-9]+\.[0-9]+)\..*/\1/')
QEMUGDB = $(shell if $(QEMU) -help | grep -q '^-gdb'; \
	then echo "-gdb tcp::$(GDBPORT)"; \
	else echo "-s -p $(GDBPORT)"; fi)

QEMUOPTS := -machine virt -bios none -kernel kernel/kernel -m 128M -smp $(CPUS) -nographic
QEMUOPTS += -global virtio-mmio.force-legacy=false
QEMUOPTS += -drive file=fs.img,if=none,format=raw,id=x0
QEMUOPTS += -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0

check-qemu-version:
	@if [ "$(shell echo "$(QEMU_VERSION) >= $(MIN_QEMU_VERSION)" | bc)" -eq 0 ]; then \
		echo "ERROR: Need qemu version >= $(MIN_QEMU_VERSION)"; \
		exit 1; \
	fi

image: kernel/kernel fs.img

qemu: check-qemu-version image
	$(QEMU) $(QEMUOPTS)

qemu-gdb: image .gdbinit
	@echo "*** Now run 'gdb' in another window." 1>&2
	$(QEMU) $(QEMUOPTS) -S $(QEMUGDB)

run: qemu
gdb: qemu-gdb

.PHONY: check-qemu-version image qemu qemu-gdb run gdb
