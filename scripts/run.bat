@echo off
echo === NexusRTOS on QEMU (Ctrl+A then X to quit) ===
qemu-system-arm -M lm3s6965evb -nographic -kernel "%~dp0..\nexus-rtos.elf"
