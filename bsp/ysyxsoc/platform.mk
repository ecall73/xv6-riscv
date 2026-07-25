ifneq ($(ARCH),riscv32imac-ysyxsoc)
  $(error bsp/ysyxsoc supports ARCH=riscv32imac-ysyxsoc, not $(ARCH))
endif

ARCH_CFLAGS := -march=rv32imac_zicsr_zifencei -mabi=ilp32
KERNEL_LDSCRIPT := bsp/ysyxsoc/kernel.ld
BSP_OBJS := \
  bsp/ysyxsoc/boot.o \
  bsp/ysyxsoc/uart.o \
  bsp/ysyxsoc/platform.o \
  bsp/common/ramdisk.o \
  bsp/common/fsimg.o

NPC_HOME ?= $(abspath ../npc)
ifeq ($(wildcard $(NPC_HOME)/csrc/npc-main.c),)
  $(error NPC_HOME=$(NPC_HOME) must point to an NPC repository)
endif

YSYXSOC_BUILD_DIR := build/$(ARCH)
YSYXSOC_IMAGE := $(YSYXSOC_BUILD_DIR)/kernel.bin
YSYXSOCFLAGS := -l $(abspath $(YSYXSOC_BUILD_DIR)/npc-log.txt)
YSYXSOCFLAGS += -f $(abspath kernel/kernel)
YSYXSOCFLAGS += -F $(abspath $(YSYXSOC_BUILD_DIR)/npc-ftrace.txt)
YSYXSOCFLAGS += -E $(abspath $(YSYXSOC_BUILD_DIR)/npc-etrace.txt)
YSYXSOCFLAGS += -M $(abspath $(YSYXSOC_BUILD_DIR)/npc-mtrace.txt)
YSYXSOCFLAGS += -D $(abspath $(YSYXSOC_BUILD_DIR)/npc-dtrace.txt)
ifeq ($(filter 1 y yes true,$(DEBUG)),)
YSYXSOCFLAGS += -b
endif

image: kernel/kernel
	@mkdir -p $(YSYXSOC_BUILD_DIR)
	$(OBJCOPY) -S -O binary $< $(YSYXSOC_IMAGE)

run: image
	$(MAKE) -C $(NPC_HOME) SIM_MODE=ysyxsoc sim \
		ARGS="$(YSYXSOCFLAGS)" IMG=$(abspath $(YSYXSOC_IMAGE)) DEBUG=$(DEBUG)

gdb: image
	$(MAKE) -C $(NPC_HOME) SIM_MODE=ysyxsoc gdb \
		ARGS="$(YSYXSOCFLAGS)" IMG=$(abspath $(YSYXSOC_IMAGE)) DEBUG=1

.PHONY: image run gdb
