# NexusRTOS - Real-Time Operating System for ARM Cortex-M
# Copyright (c) 2026 - MIT License (see LICENSE)

PREFIX  = arm-none-eabi-
CC      = $(PREFIX)gcc
AS      = $(PREFIX)gcc
LD      = $(PREFIX)gcc
OBJCOPY = $(PREFIX)objcopy
SIZE    = $(PREFIX)size
GDB     = $(PREFIX)gdb
QEMU    = qemu-system-arm

TARGET  = nexus-rtos

CFLAGS  = -mcpu=cortex-m3 -mthumb -Wall -Wextra -Os -g \
          -ffreestanding -nostdlib -Iinclude
ASFLAGS = -mcpu=cortex-m3 -mthumb -g
LDFLAGS = -mcpu=cortex-m3 -mthumb -nostdlib -T linker.ld \
          -Wl,-Map=$(TARGET).map,--gc-sections

C_SRCS  = src/startup.c   \
          src/scheduler.c  \
          src/task.c       \
          src/sysclock.c   \
          src/mutex.c      \
          src/semaphore.c  \
          src/msgqueue.c   \
          src/heap.c       \
          src/uart.c       \
          src/shell.c      \
          src/ktrace.c     \
          src/fault.c      \
          src/main.c

S_SRCS  = src/context_switch.s

OBJS    = $(C_SRCS:.c=.o) $(S_SRCS:.s=.o)

# ── Rules ──────────────────────────────────────────────────────────────
all: $(TARGET).bin
	$(SIZE) $(TARGET).elf

$(TARGET).elf: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.s
	$(AS) $(ASFLAGS) -c -o $@ $<

# ── Run / Debug ────────────────────────────────────────────────────────
run: $(TARGET).elf
	$(QEMU) -M lm3s6965evb -nographic -kernel $(TARGET).elf

debug: $(TARGET).elf
	$(QEMU) -M lm3s6965evb -nographic -kernel $(TARGET).elf -S -gdb tcp::3333 &
	$(GDB) $(TARGET).elf -x .gdbinit

clean:
	rm -f $(OBJS) $(TARGET).elf $(TARGET).bin $(TARGET).map

.PHONY: all run debug clean
