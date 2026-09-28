#!/usr/bin/env bash
# NexusRTOS - Debug mode, GDB on port 3333
set -e
DIR="$(cd "$(dirname "$0")/.." && pwd)"
echo "NexusRTOS debug session..."
echo "In another terminal:  arm-none-eabi-gdb $DIR/nexus-rtos.elf -x $DIR/.gdbinit"
qemu-system-arm -M lm3s6965evb -nographic -kernel "$DIR/nexus-rtos.elf" -S -gdb tcp::3333
