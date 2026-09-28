@echo off
echo === NexusRTOS Debug Mode (GDB server on port 3333) ===
echo Open another terminal and run:
echo   arm-none-eabi-gdb ..\nexus-rtos.elf -x ..\.gdbinit
echo.
qemu-system-arm -M lm3s6965evb -nographic -kernel "%~dp0..\nexus-rtos.elf" -S -gdb tcp::3333
