ifneq ($(filter riscv32-nemu riscv32imac-nemu,$(ARCH)),$(ARCH))
  $(error bsp/nemu supports ARCH=riscv32-nemu or riscv32imac-nemu, not $(ARCH))
endif

ifeq ($(ARCH),riscv32-nemu)
ARCH_CFLAGS := -march=rv32gc_zicsr_zifencei -mabi=ilp32d
else
ARCH_CFLAGS := -march=rv32imac_zicsr_zifencei -mabi=ilp32
endif

BSP_OBJS := \
  bsp/nemu/uart.o \
  bsp/nemu/platform.o \
  bsp/common/ramdisk.o \
  bsp/common/fsimg.o

NEMU_HOME ?= $(abspath ../nemu)
ifeq ($(wildcard $(NEMU_HOME)/src/monitor/monitor.c),)
  $(error NEMU_HOME=$(NEMU_HOME) must point to a NEMU repository)
endif

NEMU_ISA := riscv32
NEMU_BUILD_DIR := build/$(ARCH)
NEMU_IMAGE := $(NEMU_BUILD_DIR)/kernel.bin
NEMUFLAGS := -l $(abspath $(NEMU_BUILD_DIR)/nemu-log.txt)
NEMUFLAGS += -f $(abspath kernel/kernel)
NEMUFLAGS += --ftrace-log=$(abspath $(NEMU_BUILD_DIR)/nemu-ftrace.txt)
NEMUFLAGS += --etrace-log=$(abspath $(NEMU_BUILD_DIR)/nemu-etrace.txt)
NEMUFLAGS += --mtrace-log=$(abspath $(NEMU_BUILD_DIR)/nemu-mtrace.txt)
NEMUFLAGS += --dtrace-log=$(abspath $(NEMU_BUILD_DIR)/nemu-dtrace.txt)
ifeq ($(filter 1 y yes true,$(DEBUG)),)
NEMUFLAGS += -b
endif

image: kernel/kernel
	@mkdir -p $(NEMU_BUILD_DIR)
	$(OBJCOPY) -S -O binary $< $(NEMU_IMAGE)

run: image
	$(MAKE) -C $(NEMU_HOME) ISA=$(NEMU_ISA) git_commit= run \
		ARGS="$(NEMUFLAGS)" IMG=$(abspath $(NEMU_IMAGE))

gdb: image
	$(MAKE) -C $(NEMU_HOME) ISA=$(NEMU_ISA) git_commit= gdb \
		ARGS="$(NEMUFLAGS)" IMG=$(abspath $(NEMU_IMAGE))

.PHONY: image run gdb
