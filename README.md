<p align="center">
  <h1 align="center">NexusRTOS</h1>
  <p align="center">
    A preemptive real-time operating system kernel for ARM Cortex-M3
    <br />
    Written from scratch in C and ARM assembly &mdash; runs entirely on QEMU
  </p>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/arch-ARM_Cortex--M3-blue" alt="Architecture" />
  <img src="https://img.shields.io/badge/lang-C_|_ARM_ASM-green" alt="Language" />
  <img src="https://img.shields.io/badge/platform-QEMU-orange" alt="Platform" />
  <img src="https://img.shields.io/badge/license-MIT-lightgrey" alt="License" />
</p>

---

## Overview

NexusRTOS is an RTOS kernel built from the ground up for the ARM Cortex-M3 (TI LM3S6965 Stellaris). It runs entirely in QEMU -- no physical hardware required.

The kernel provides preemptive multitasking, synchronization primitives, dynamic memory, fault recovery, and runtime instrumentation, all in about 2000 lines of freestanding C and ARM Thumb-2 assembly with zero library dependencies.

### Features

| Component | Description |
|:--|:--|
| **Scheduler** | Priority-based preemptive scheduling with round-robin at equal priority levels |
| **Context Switch** | PendSV-based, hand-written ARM Thumb-2 assembly |
| **Mutexes** | Binary lock with priority inheritance protocol |
| **Semaphores** | Counting semaphore with blocking wait/post |
| **Message Queues** | Fixed-size ring-buffer IPC with blocking send/receive |
| **Heap Allocator** | First-fit with block splitting and forward coalescing |
| **KTrace** | Lockless ring-buffer kernel event tracer (context switches, mutex ops, faults) |
| **Fault Recovery** | Stack canary detection, HardFault handling, and automatic task restart |
| **CPU Profiler** | Per-task run-time tracking with `top`-style shell output |
| **UART Driver** | Polled UART0 with minimal `printf` (no libc) |
| **Debug Shell** | Interactive CLI: `ps`, `top`, `free`, `uptime`, `trace`, `fault`, `crash` |

---

## Quick Start

### Prerequisites

| Tool | Purpose |
|:--|:--|
| `arm-none-eabi-gcc` | ARM cross-compiler |
| `qemu-system-arm` | Cortex-M3 emulator |
| `make` | Build system |
| `arm-none-eabi-gdb` | Debugger (optional) |

<details>
<summary><strong>Installation</strong></summary>

**MSYS2 (Windows):**
```bash
pacman -S mingw-w64-x86_64-arm-none-eabi-gcc mingw-w64-x86_64-qemu-system-arm make
```

**Ubuntu / WSL:**
```bash
sudo apt install gcc-arm-none-eabi qemu-system-arm make
```

**macOS:**
```bash
brew install arm-none-eabi-gcc qemu
```
</details>

### Build & Run

```bash
make        # compile
make run    # launch on QEMU
```

Press `Ctrl+A` then `X` to exit QEMU.

### Example Session

```
  _   _                     ____  _____ ___  ____
 | \ | | _____  ___   _ ___|  _ \|_   _/ _ \/ ___|
 |  \| |/ _ \ \/ / | | / __| |_) | | || | | \___ \
 | |\  |  __/>  <| |_| \__ \  _ <  | || |_| |___) |
 |_| \_|\___/_/\_\\__,_|___/_| \_\ |_|  \___/|____/

 v1.0.0 | ARM Cortex-M3 | MIT License
 -----------------------------------------------

 Initializing kernel...
 7 tasks created. Starting scheduler...
 [KTrace] Kernel event tracer active
 [Fault]  Stack canary monitor active

nexus> ps
ID  NAME            PRI  STATE  STACK%  FAULTS
--  --------------  ---  -----  ------  ------
0   idle            0    READY  0%      0
1   heartbeat       2    BLOCK  8%      0
2   producer        3    BLOCK  15%     0
3   consumer        3    BLOCK  10%     0
4   stats           1    BLOCK  5%      0
5   shell           4    RUN    20%     0
6   stress          1    BLOCK  2%      0

nexus> top
ID  NAME            CPU%   SWITCHES
--  --------------  -----  --------
0   idle            72%    1580
1   heartbeat       3%     12
2   producer        5%     15
3   consumer        4%     10
4   stats           1%     2
5   shell           14%    45
6   stress          1%     130
(uptime: 12845 ticks)

nexus> crash
Triggering deliberate stack overflow on 'stress' task...

[FAULT] Stack overflow: task 'stress' (id=6)
[FAULT] Task 'stress' auto-restarted (crash #1)

nexus> fault
TICK       TYPE       TID  PC          LR
---------  ---------  ---  ----------  ----------
12845      STK_OVF    6    0x0         0x0
(1 faults total)

nexus> trace
TICK       EVENT      TID  EXTRA
---------  --------   ---  -----
12840      SWITCH       5    0
12841      MTX_LOCK     5    0
12842      DELAY        5    100
12843      SWITCH       1    5
12844      STK_OVF      6    0
12845      RESTART      6    1
(6 events total, showing last 6)
```

---

## Architecture

```
                    +------------------+
                    |     main.c       |  Application layer
                    |  (demo tasks)    |
                    +--------+---------+
                             |
          +------------------+------------------+
          |                  |                  |
   +------+------+   +------+------+   +-------+------+
   |   mutex.c   |   | semaphore.c |   |  msgqueue.c  |
   |  (pri-inh)  |   |  (counting) |   |  (ring buf)  |
   +------+------+   +------+------+   +-------+------+
          |                  |                  |
          +------------------+------------------+
                             |
               +-------------+-------------+
               |             |             |
      +--------+--------+   |   +---------+--------+
      |   scheduler.c   |   |   |    ktrace.c      |
      |  (ready queues) |   |   | (event tracer)   |
      |  (CPU profiler) |   |   +------------------+
      +--------+---------+  |
               |             |
               |   +---------+--------+
               |   |    fault.c       |
               |   | (canary checks)  |
               |   | (auto-recovery)  |
               |   +------------------+
               |
  +------------+-------------+
  |                          |
  +--------+--------+ +-----+----------+
  | context_switch.s | |   sysclock.c   |
  | (PendSV handler) | | (SysTick ISR) |
  | (HardFault hndl) | | (canary scan) |
  +--------+---------+ +-------+-------+
           |                    |
  +--------+-------------------+--------+
  |              Hardware Abstraction   |
  |  startup.c  |  uart.c  |  cortex_m3.h  |
  +---------------------------------------------+
  |         ARM Cortex-M3 (QEMU LM3S6965)      |
  +---------------------------------------------+
```

---

## Project Structure

```
nexus-rtos/
├── Makefile              Build system
├── linker.ld             Memory layout for LM3S6965
├── LICENSE               MIT License
├── .gdbinit              GDB auto-connect script
│
├── include/
│   ├── os_config.h       Kernel configuration (tasks, stack, canaries, trace)
│   ├── cortex_m3.h       Register definitions & intrinsics
│   ├── task.h            Task Control Block & API
│   ├── scheduler.h       Scheduler API
│   ├── mutex.h           Mutex with priority inheritance
│   ├── semaphore.h       Counting semaphore
│   ├── msgqueue.h        Message queue (IPC)
│   ├── heap.h            Dynamic memory allocator
│   ├── sysclock.h        System clock API
│   ├── uart.h            UART driver API
│   ├── shell.h           Debug shell
│   ├── ktrace.h          Kernel event tracer
│   └── fault.h           Fault detection & recovery
│
├── src/
│   ├── startup.c         Vector table & C runtime init
│   ├── context_switch.s  PendSV + HardFault handlers (ARM Thumb-2)
│   ├── scheduler.c       Kernel core, ready queues, CPU time accounting
│   ├── task.c            Task creation (with stack canaries), delay, yield
│   ├── sysclock.c        SysTick ISR, tick management, canary scanning
│   ├── mutex.c           Mutex with priority inheritance
│   ├── semaphore.c       Counting semaphore
│   ├── msgqueue.c        Ring-buffer message queue
│   ├── heap.c            First-fit allocator
│   ├── uart.c            UART0 driver with minimal printf
│   ├── shell.c           Debug shell (ps, top, trace, fault, crash)
│   ├── ktrace.c          Lockless ring-buffer event tracer
│   ├── fault.c           Stack overflow detection & auto-recovery
│   └── main.c            Demo application
│
├── scripts/              Launch scripts (.bat / .sh)
└── notes/
    └── notes.pdf         23-chapter reference guide
```

---

## Shell Commands

| Command | Description |
|:--|:--|
| `ps` | List tasks with priority, state, stack usage, and fault count |
| `top` | Per-task CPU usage and context-switch count |
| `free` | Heap memory usage |
| `uptime` | System uptime |
| `trace` | Dump the last 32 kernel trace events |
| `fault` | Fault history log |
| `crash` | Trigger a test stack overflow (stress task auto-recovers) |
| `version` | Kernel version and target |
| `help` | List commands |

---

## Design Notes

### Context Switching

The PendSV handler (set to the lowest exception priority) performs context switches:

1. Hardware saves `{R0-R3, R12, LR, PC, xPSR}` onto the current task's stack (PSP)
2. PendSV saves `{R4-R11}` via `STMDB`
3. `os_schedule()` selects the highest-priority ready task and accounts CPU time
4. KTrace records a `SWITCH` event
5. PendSV restores `{R4-R11}` via `LDMIA`
6. Exception return (`BX LR` with `EXC_RETURN = 0xFFFFFFFD`) restores the hardware frame

### Fault Recovery

1. **Prevention:** Stack canaries (`0xDEADBEEF x 4`) painted at the bottom of every task stack
2. **Detection:** SysTick ISR checks one task's canaries per tick (round-robin)
3. **Diagnosis:** Logs fault record (tick, PC, LR, task ID) and KTrace event
4. **Recovery:** Terminates faulting task, re-initializes its stack, restarts from entry point if marked restartable
5. **HardFault path:** Assembly wrapper extracts the exception frame (MSP/PSP), C handler logs diagnostics and redirects to the recovery stub

### Priority Inheritance

When a high-priority task blocks on a mutex held by a lower-priority task, the holder's priority is temporarily boosted to prevent unbounded priority inversion. Priority reverts on unlock.

### Memory Layout

| Region | Address | Size |
|:--|:--|:--|
| Flash | `0x00000000` | 256 KB |
| SRAM | `0x20000000` | 64 KB |

---

## Debugging

```bash
make debug    # starts QEMU paused + GDB
```

Useful GDB commands:

```gdb
break PendSV_Handler            # context switch
break HardFault_Handler_C       # fault recovery
print current_task->name        # running task
print current_task->run_ticks   # CPU time
x/4x &task_pool[0].stack[0]    # stack canaries
watch current_task              # break on switch
```

---

## Configuration

All tunables live in [`include/os_config.h`](include/os_config.h):

| Define | Default | Description |
|:--|:--|:--|
| `MAX_TASKS` | 16 | Maximum concurrent tasks |
| `MAX_PRIORITY_LEVELS` | 8 | Priority levels (0 = lowest) |
| `STACK_SIZE` | 512 words | Per-task stack (2 KB) |
| `HEAP_SIZE` | 16 KB | Dynamic memory pool |
| `SYSTICK_FREQ_HZ` | 1000 | Tick rate (1 ms) |
| `TRACE_BUF_SIZE` | 256 | KTrace ring buffer entries |
| `STACK_CANARY` | `0xDEADBEEF` | Canary word value |
| `STACK_CANARY_WORDS` | 4 | Canary words per task |

---

## License

MIT License. See [LICENSE](LICENSE) for details.
