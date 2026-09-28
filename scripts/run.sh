#!/usr/bin/env bash
# NexusRTOS - press Ctrl+A then X to quit
set -e
DIR="$(cd "$(dirname "$0")/.." && pwd)"
qemu-system-arm -M lm3s6965evb -nographic -kernel "$DIR/nexus-rtos.elf"
