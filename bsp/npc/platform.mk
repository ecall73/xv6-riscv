ifneq ($(ARCH),riscv32imac-npc)
  $(error bsp/npc supports ARCH=riscv32imac-npc, not $(ARCH))
endif

ARCH_CFLAGS := -march=rv32imac_zicsr_zifencei -mabi=ilp32
BSP_OBJS := \
  bsp/npc/uart.o \
  bsp/npc/platform.o \
  bsp/common/ramdisk.o \
  bsp/common/fsimg.o

NPC_HOME ?= $(abspath ../npc)
ifeq ($(wildcard $(NPC_HOME)/csrc/npc-main.c),)
  $(error NPC_HOME=$(NPC_HOME) must point to an NPC repository)
endif

NPC_BUILD_DIR := build/$(ARCH)
NPC_IMAGE := $(NPC_BUILD_DIR)/kernel.bin
NPCFLAGS := -l $(abspath $(NPC_BUILD_DIR)/npc-log.txt)
NPCFLAGS += -f $(abspath kernel/kernel)
NPCFLAGS += -F $(abspath $(NPC_BUILD_DIR)/npc-ftrace.txt)
NPCFLAGS += -E $(abspath $(NPC_BUILD_DIR)/npc-etrace.txt)
NPCFLAGS += -M $(abspath $(NPC_BUILD_DIR)/npc-mtrace.txt)
NPCFLAGS += -D $(abspath $(NPC_BUILD_DIR)/npc-dtrace.txt)
ifeq ($(filter 1 y yes true,$(DEBUG)),)
NPCFLAGS += -b
endif

image: kernel/kernel
	@mkdir -p $(NPC_BUILD_DIR)
	$(OBJCOPY) -S -O binary $< $(NPC_IMAGE)

run: image
	$(MAKE) -C $(NPC_HOME) SIM_MODE=npc sim \
		ARGS="$(NPCFLAGS)" IMG=$(abspath $(NPC_IMAGE)) DEBUG=$(DEBUG)

gdb: image
	$(MAKE) -C $(NPC_HOME) SIM_MODE=npc gdb \
		ARGS="$(NPCFLAGS)" IMG=$(abspath $(NPC_IMAGE)) DEBUG=1

.PHONY: image run gdb
