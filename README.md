<p align="center">
  <h1 align="center">NexusRTOS</h1>
  <p align="center">
    A fault-resilient, preemptive real-time operating system kernel for ARM Cortex-M3
    <br />
    Written from scratch in C and ARM assembly &mdash; runs entirely on QEMU
  </p>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/arch-ARM_Cortex--M3-blue" alt="Architecture" />
  <img src="https://img.shields.io/badge/lang-C_|_ARM_ASM-green" alt="Language" />
  <img src="https://img.shields.io/badge/platform-QEMU-orange" alt="Platform" />
  <img src="https://img.shields.io/badge/license-MIT-lightgrey" alt="License" />
  <img src="https://img.shields.io/badge/features-KTrace_|_Fault_Recovery_|_CPU_Profiler-purple" alt="Novel Features" />
</p>

---

> **Resume line:** *Designed NexusRTOS, a fault-resilient RTOS kernel for ARM Cortex-M3 featuring kernel event tracing, stack-overflow detection with auto-recovery, priority-inheritance mutexes, and per-task CPU profiling*

---

## Overview

NexusRTOS is a fully-functional RTOS kernel designed and implemented from the ground up. It targets the ARM Cortex-M3 (TI LM3S6965 Stellaris) and runs entirely in QEMU with no physical hardware required.

Beyond a standard teaching RTOS, NexusRTOS implements three **novel subsystems** that demonstrate systems engineering depth:

### Novel Features

| Feature | What it does | Why it matters |
|:--|:--|:--|
| **KTrace** (Kernel Event Tracer) | Lockless ring-buffer that logs context switches, mutex operations, task lifecycle events, and ISR activity with microsecond timestamps | Production RTOS kernels (FreeRTOS, Zephyr) have similar trace hooks -- this implements one from scratch, showing observability design |
| **Fault Isolation & Auto-Recovery** | Stack canary painting (`0xDEADBEEF`), round-robin canary scanning in SysTick ISR, HardFault handler with diagnostic logging, and automatic restart of crashed tasks | Demonstrates safety-critical concepts: memory corruption detection, graceful degradation, and supervisor-mode recovery |
| **Per-Task CPU Profiler** | Tracks cumulative run ticks per task in the scheduler, with a `top` command showing CPU% and context-switch counts | Shows real-time performance analysis capability -- a common interview discussion topic |

### Core Kernel Features

| Component | Implementation |
|:--|:--|
| **Scheduler** | Priority-based preemptive scheduling with round-robin at equal priority levels |
| **Context Switch** | PendSV-based, hand-written ARM Thumb-2 assembly |
| **Mutexes** | Binary lock with priority inheritance protocol |
| **Semaphores** | Counting semaphore with blocking wait/post |
| **Message Queues** | Fixed-size ring-buffer IPC with blocking send/receive |
| **Memory Allocator** | First-fit heap with block splitting and forward coalescing |
| **UART Driver** | Polled UART0 with minimal `printf` (no libc dependency) |
| **Debug Shell** | Interactive CLI: `ps`, `top`, `free`, `uptime`, `trace`, `fault`, `crash`, `version` |
| **Idle Task** | WFI-based low-power idle loop |

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
<summary><strong>Installation (click to expand)</strong></summary>

**MSYS2 (Windows, recommended):**
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

### Demo: Fault Recovery in Action

```
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
├── .gitignore
│
├── include/
│   ├── os_config.h       Tunables (tasks, stack, canaries, trace buffer)
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
│   ├── ktrace.h          Kernel event tracer API
│   └── fault.h           Fault detection & recovery API
│
├── src/
│   ├── startup.c         Vector table & C runtime init
│   ├── context_switch.s  PendSV + HardFault handlers (ARM Thumb-2)
│   ├── scheduler.c       Kernel core, ready queues, CPU profiling
│   ├── task.c            Task creation (with stack canaries), delay, yield
│   ├── sysclock.c        SysTick ISR, tick management, canary scanning
│   ├── mutex.c           Mutex with priority inheritance + trace events
│   ├── semaphore.c       Counting semaphore
│   ├── msgqueue.c        Ring-buffer message queue
│   ├── heap.c            First-fit allocator
│   ├── uart.c            UART0 driver with minimal printf
│   ├── shell.c           Interactive debug shell (ps, top, trace, fault, crash)
│   ├── ktrace.c          Lockless ring-buffer kernel event tracer
│   ├── fault.c           Stack overflow detection & auto-recovery
│   └── main.c            Demo application with stress test
│
├── scripts/              Launch scripts (.bat / .sh)
└── notes/
    └── notes.pdf         Comprehensive learning guide (20 chapters)
```

---

## Shell Commands

| Command | Description |
|:--|:--|
| `ps` | List active tasks with priority, state, stack usage, and fault count |
| `top` | Per-task CPU usage percentage and context-switch count |
| `free` | Heap memory usage statistics |
| `uptime` | System uptime in minutes/seconds/ticks |
| `trace` | Dump the last 32 kernel trace events |
| `fault` | Show fault history log (stack overflows and hard faults) |
| `crash` | Trigger a deliberate stack overflow on the stress task (demonstrates auto-recovery) |
| `version` | Kernel version and target info |
| `help` | List available commands |

---

## Debugging

```bash
# Terminal 1: start QEMU paused
make debug

# Terminal 2: connect GDB
arm-none-eabi-gdb nexus-rtos.elf -x .gdbinit
```

Useful GDB commands:

```gdb
break PendSV_Handler            # break on context switch
break HardFault_Handler_C       # break on fault recovery
print current_task->name        # which task is running
print current_task->run_ticks   # CPU time consumed
print task_pool[0].fault_count  # faults for task 0
x/4x &task_pool[0].stack[0]    # inspect stack canaries
watch current_task              # break on task switch
```

---

## Technical Details

### Context Switching

The PendSV handler (lowest exception priority) performs cooperative/preemptive context switches:

1. Hardware saves `{R0-R3, R12, LR, PC, xPSR}` onto the current task's stack (PSP)
2. PendSV manually saves `{R4-R11}` via `STMDB`
3. Scheduler selects the highest-priority ready task, logs CPU time
4. KTrace records a `SWITCH` event with old/new task IDs
5. PendSV restores `{R4-R11}` from the new task's stack via `LDMIA`
6. Exception return (`BX LR` with `EXC_RETURN = 0xFFFFFFFD`) restores the hardware frame

### Fault Recovery Pipeline

1. **Prevention:** Stack canaries (`0xDEADBEEF x 4`) painted at the bottom of every task stack on creation
2. **Detection:** SysTick ISR scans one task's canaries per tick (round-robin across all tasks)
3. **Diagnosis:** On corruption, logs fault record (tick, PC, LR, task ID, type) and KTrace event
4. **Recovery:** Terminates faulting task, re-initializes its stack, restores canaries, restarts from entry point
5. **HardFault path:** Assembly wrapper extracts fault frame (MSP/PSP), C handler logs diagnostics and redirects to recovery stub

### Priority Inheritance

When a high-priority task blocks on a mutex held by a lower-priority task, the holder's priority is temporarily boosted to prevent priority inversion (the Mars Pathfinder bug). Priority reverts on unlock.

### Memory Layout

| Region | Address | Size | Contents |
|:--|:--|:--|:--|
| Flash | `0x00000000` | 256 KB | Vector table, code, constants |
| SRAM | `0x20000000` | 64 KB | Data, BSS, heap, task stacks |

---

## License

MIT License. See [LICENSE](LICENSE) for details.
