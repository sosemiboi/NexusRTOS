"""Generate notes.pdf -- comprehensive learning guide for NexusRTOS."""

from fpdf import FPDF

class PDF(FPDF):
    def header(self):
        self.set_font("Helvetica", "B", 10)
        self.set_text_color(100, 100, 100)
        self.cell(0, 8, "NexusRTOS - Learning Guide", align="R", new_x="LMARGIN", new_y="NEXT")
        self.line(10, self.get_y(), 200, self.get_y())
        self.ln(4)

    def footer(self):
        self.set_y(-15)
        self.set_font("Helvetica", "I", 8)
        self.set_text_color(128, 128, 128)
        self.cell(0, 10, f"Page {self.page_no()}/{{nb}}", align="C")

    def chapter_title(self, title):
        self.set_font("Helvetica", "B", 18)
        self.set_text_color(30, 60, 120)
        self.cell(0, 14, title, new_x="LMARGIN", new_y="NEXT")
        self.line(10, self.get_y(), 200, self.get_y())
        self.ln(6)

    def section(self, title):
        self.set_font("Helvetica", "B", 13)
        self.set_text_color(40, 80, 140)
        self.cell(0, 10, title, new_x="LMARGIN", new_y="NEXT")
        self.ln(2)

    def body(self, text):
        self.set_font("Helvetica", "", 10)
        self.set_text_color(30, 30, 30)
        self.multi_cell(0, 5.5, text)
        self.ln(3)

    def code(self, text):
        self.set_font("Courier", "", 8.5)
        self.set_fill_color(240, 240, 240)
        self.set_text_color(30, 30, 30)
        for line in text.split("\n"):
            self.cell(0, 4.5, "  " + line, fill=True, new_x="LMARGIN", new_y="NEXT")
        self.ln(4)

    def bullet(self, text):
        self.set_font("Helvetica", "", 10)
        self.set_text_color(30, 30, 30)
        x = self.get_x()
        self.cell(6, 5.5, "-")
        self.multi_cell(0, 5.5, text)
        self.ln(1)


def build_pdf():
    pdf = PDF()
    pdf.alias_nb_pages()
    pdf.set_auto_page_break(auto=True, margin=20)

    # ── TITLE PAGE ──────────────────────────────────────────────────
    pdf.add_page()
    pdf.ln(50)
    pdf.set_font("Helvetica", "B", 32)
    pdf.set_text_color(20, 50, 100)
    pdf.cell(0, 16, "NexusRTOS", align="C", new_x="LMARGIN", new_y="NEXT")
    pdf.set_font("Helvetica", "", 16)
    pdf.set_text_color(80, 80, 80)
    pdf.cell(0, 10, "A Complete Learning Guide", align="C", new_x="LMARGIN", new_y="NEXT")
    pdf.ln(8)
    pdf.set_font("Helvetica", "", 12)
    pdf.cell(0, 8, "ARM Cortex-M3  |  C + Assembly  |  QEMU", align="C", new_x="LMARGIN", new_y="NEXT")
    pdf.ln(20)
    pdf.set_font("Helvetica", "I", 10)
    pdf.set_text_color(120, 120, 120)
    pdf.cell(0, 8, "Build a real-time operating system kernel from zero.", align="C", new_x="LMARGIN", new_y="NEXT")
    pdf.cell(0, 8, "No hardware needed -- runs entirely on QEMU.", align="C", new_x="LMARGIN", new_y="NEXT")

    # ── TABLE OF CONTENTS ───────────────────────────────────────────
    pdf.add_page()
    pdf.chapter_title("Table of Contents")
    toc = [
        "1. What is an RTOS and Why Build One?",
        "2. ARM Cortex-M3 Architecture Crash Course",
        "3. Toolchain Setup (compiler, QEMU, GDB)",
        "4. Project Structure Walkthrough",
        "5. Startup Code & Vector Table",
        "6. The Linker Script",
        "7. Context Switching Deep Dive",
        "8. The Scheduler",
        "9. Task Management",
        "10. SysTick Timer & System Clock",
        "11. Mutexes & Priority Inheritance",
        "12. Semaphores",
        "13. Message Queues (IPC)",
        "14. Heap Allocator",
        "15. UART Driver & printf",
        "16. Debug Shell",
        "17. Building & Running",
        "18. Debugging with GDB",
        "19. KTrace: Kernel Event Tracer",
        "20. Fault Isolation & Auto-Recovery",
        "21. Per-Task CPU Profiler",
        "22. Extending the Project",
        "23. Common Questions & Answers",
    ]
    for item in toc:
        pdf.body(item)

    # ── CH 1: What is an RTOS ───────────────────────────────────────
    pdf.add_page()
    pdf.chapter_title("1. What is an RTOS and Why Build One?")

    pdf.section("RTOS vs General-Purpose OS")
    pdf.body(
        "A Real-Time Operating System (RTOS) is designed to run tasks with "
        "deterministic timing guarantees. Unlike Linux or Windows, which optimize "
        "for throughput and fairness, an RTOS guarantees that high-priority tasks "
        "run within a bounded time (their deadline).\n\n"
        "Key differences:\n"
        "- Deterministic scheduling: worst-case response time is bounded.\n"
        "- Small footprint: typically kilobytes of RAM and flash.\n"
        "- No virtual memory / MMU: tasks share a flat address space.\n"
        "- Cooperative or preemptive multitasking on a single core.\n\n"
        "Common RTOS examples: FreeRTOS, Zephyr, ThreadX, VxWorks, QNX."
    )

    pdf.section("Why Build Your Own?")
    pdf.body(
        "Most embedded engineers *use* an RTOS but few understand the internals. "
        "Building one from scratch teaches you:\n\n"
        "1. How context switching really works at the register level.\n"
        "2. How schedulers make priority decisions in bounded time.\n"
        "3. How synchronization primitives prevent race conditions.\n"
        "4. How memory is managed without malloc/free from a C library.\n"
        "5. How hardware peripherals (timers, UARTs) interact with the kernel.\n\n"
        "This knowledge makes you dramatically more effective at debugging "
        "real embedded systems and understanding what happens beneath the "
        "abstraction layers."
    )

    # ── CH 2: ARM Cortex-M3 Architecture ────────────────────────────
    pdf.add_page()
    pdf.chapter_title("2. ARM Cortex-M3 Architecture")

    pdf.section("Why Cortex-M3?")
    pdf.body(
        "The Cortex-M3 is the workhorse of the embedded world. It is a 32-bit "
        "ARM processor designed for microcontrollers. We target the LM3S6965 "
        "(Texas Instruments Stellaris) because QEMU emulates it perfectly, so "
        "you need zero hardware.\n\n"
        "Key specs of our target:\n"
        "- CPU: ARM Cortex-M3, 50 MHz\n"
        "- Flash: 256 KB at address 0x00000000\n"
        "- SRAM: 64 KB at address 0x20000000\n"
        "- UART0 at 0x4000C000"
    )

    pdf.section("Operating Modes")
    pdf.body(
        "Cortex-M3 has two operating modes:\n\n"
        "Thread Mode: normal code execution. This is where your tasks run.\n"
        "Handler Mode: entered automatically when an exception/interrupt fires.\n\n"
        "And two stack pointers:\n"
        "MSP (Main Stack Pointer): used by exception handlers and at boot.\n"
        "PSP (Process Stack Pointer): used by tasks in our RTOS.\n\n"
        "The CONTROL register bit 1 (SPSEL) selects which stack pointer thread "
        "mode uses. Our RTOS switches to PSP for tasks, keeping MSP for the "
        "kernel/ISRs. This separation is crucial for safety."
    )

    pdf.section("Exception Model")
    pdf.body(
        "When an exception fires, the hardware automatically pushes 8 registers "
        "onto the current stack:\n\n"
        "  R0, R1, R2, R3, R12, LR, PC, xPSR\n\n"
        "This is called the 'exception frame'. The remaining registers (R4-R11) "
        "are callee-saved and must be preserved by software -- this is exactly "
        "what our PendSV context-switch handler does.\n\n"
        "Key exceptions for our RTOS:\n"
        "- SysTick (exception #15): fires every 1 ms to drive the scheduler.\n"
        "- PendSV (exception #14): triggered by software for context switches.\n"
        "  We set it to the lowest priority so it runs after all other ISRs.\n"
        "- SVC (exception #11): supervisor call, used for protected OS calls."
    )

    pdf.section("The Vector Table")
    pdf.body(
        "At address 0x00000000, the processor expects a table of 32-bit values:\n\n"
        "Offset 0x00: Initial Stack Pointer (MSP)\n"
        "Offset 0x04: Reset Handler address\n"
        "Offset 0x08: NMI Handler\n"
        "Offset 0x0C: HardFault Handler\n"
        "... and so on for each exception and interrupt.\n\n"
        "On reset, the CPU loads MSP from offset 0 and branches to the address "
        "at offset 4 (Reset_Handler). Our startup.c defines this table in the "
        ".isr_vector section, which the linker script places at address 0."
    )

    # ── CH 3: Toolchain Setup ───────────────────────────────────────
    pdf.add_page()
    pdf.chapter_title("3. Toolchain Setup")

    pdf.section("ARM Cross-Compiler")
    pdf.body(
        "We use the GNU ARM Embedded Toolchain (arm-none-eabi-gcc). The 'none' "
        "means no operating system -- we ARE the operating system.\n\n"
        "Install options:\n"
        "- Windows: MSYS2 (pacman -S mingw-w64-x86_64-arm-none-eabi-gcc) "
        "or download from developer.arm.com\n"
        "- Ubuntu/WSL: sudo apt install gcc-arm-none-eabi\n"
        "- macOS: brew install arm-none-eabi-gcc\n\n"
        "Verify: arm-none-eabi-gcc --version"
    )

    pdf.section("QEMU -- The ARM Emulator")
    pdf.body(
        "QEMU emulates the entire LM3S6965 board: CPU, memory, UART, timers. "
        "Our kernel runs on it identically to real hardware.\n\n"
        "Install:\n"
        "- Windows: MSYS2 (pacman -S mingw-w64-x86_64-qemu-system-arm) "
        "or download from qemu.org\n"
        "- Ubuntu: sudo apt install qemu-system-arm\n"
        "- macOS: brew install qemu\n\n"
        "Key QEMU flags:\n"
        "  -M lm3s6965evb    Select the board\n"
        "  -nographic        UART0 goes to terminal (no GUI window)\n"
        "  -kernel nexus-rtos.elf  Load our ELF binary\n"
        "  -S                Start paused (for GDB)\n"
        "  -gdb tcp::3333    Open GDB server on port 3333\n\n"
        "To quit QEMU: press Ctrl+A, then X."
    )

    pdf.section("GDB -- The Debugger")
    pdf.body(
        "arm-none-eabi-gdb comes with the compiler toolchain. We use it to:\n"
        "- Step through code instruction by instruction\n"
        "- Inspect registers, memory, and task stacks\n"
        "- Set breakpoints on context switches and ISRs\n"
        "- Watch variables like current_task in real time\n\n"
        "The .gdbinit file auto-connects to QEMU and sets initial breakpoints."
    )

    # ── CH 4: Project Structure ─────────────────────────────────────
    pdf.add_page()
    pdf.chapter_title("4. Project Structure Walkthrough")

    pdf.body(
        "The project is organized to separate concerns clearly:\n\n"
        "include/ -- All header files. Each kernel component has one.\n"
        "src/ -- All implementation files.\n"
        "scripts/ -- Convenience scripts for running and debugging.\n"
        "notes/ -- This PDF and the script that generates it.\n\n"
        "Key files and what they do:"
    )

    files = [
        ("os_config.h", "Tunable constants: MAX_TASKS, STACK_SIZE, HEAP_SIZE, clock speeds. Change these to experiment."),
        ("cortex_m3.h", "Hardware register addresses (SysTick, SCB, UART, NVIC) and inline assembly intrinsics (__disable_irq, __set_PSP, etc)."),
        ("startup.c", "Vector table and Reset_Handler. Copies .data from flash to RAM, zeros .bss, calls main()."),
        ("linker.ld", "Tells the linker where to put code (flash) and data (SRAM). Defines _estack, _sdata, _sbss, etc."),
        ("context_switch.s", "The heart of the RTOS: PendSV_Handler in ARM Thumb assembly. Saves/restores task contexts."),
        ("scheduler.c", "Kernel core: ready queues (one per priority level), os_schedule(), os_init(), os_start()."),
        ("task.c", "Task creation (stack initialization), deletion, delay, yield."),
        ("sysclock.c", "SysTick_Handler ISR: increments tick counter, wakes delayed tasks, triggers PendSV."),
        ("mutex.c", "Mutex with priority inheritance protocol."),
        ("semaphore.c", "Counting semaphore with blocking wait/post."),
        ("msgqueue.c", "Fixed-size ring-buffer message queue with blocking send/receive."),
        ("heap.c", "First-fit heap allocator with block splitting and forward coalescing."),
        ("uart.c", "UART0 driver for the LM3S6965: init, putc, getc, puts, printf."),
        ("shell.c", "Interactive debug shell: ps, top, free, uptime, trace, fault, crash, version, help."),
        ("ktrace.c", "Lockless ring-buffer kernel event tracer. Logs context switches, mutex ops, faults."),
        ("fault.c", "Stack canary checking, HardFault C handler, auto-restart of crashed tasks."),
        ("main.c", "Demo application with producer/consumer, stats, shell, and stress-test tasks."),
    ]
    for name, desc in files:
        pdf.set_font("Helvetica", "B", 10)
        pdf.set_text_color(30, 30, 30)
        pdf.cell(0, 6, name, new_x="LMARGIN", new_y="NEXT")
        pdf.set_font("Helvetica", "", 9.5)
        pdf.set_text_color(60, 60, 60)
        pdf.multi_cell(0, 5, desc)
        pdf.ln(2)

    # ── CH 5: Startup Code ──────────────────────────────────────────
    pdf.add_page()
    pdf.chapter_title("5. Startup Code & Vector Table")

    pdf.body(
        "When the Cortex-M3 comes out of reset, it does exactly two things:\n"
        "1. Loads the Main Stack Pointer (MSP) from address 0x00000000.\n"
        "2. Branches to the address at 0x00000004 (the Reset_Handler).\n\n"
        "Our startup.c provides both: a vector table in the .isr_vector section "
        "(placed at address 0 by the linker script), and the Reset_Handler function."
    )

    pdf.section("The Reset Handler")
    pdf.body(
        "Reset_Handler does the C runtime setup that a normal OS would do for you:\n\n"
        "1. Copy initialized global variables (.data section) from flash to SRAM. "
        "The linker stores them in flash (read-only), but the CPU expects them in "
        "RAM (read-write). The symbols _sidata, _sdata, _edata mark the source and "
        "destination.\n\n"
        "2. Zero the .bss section (uninitialized globals). C guarantees these start "
        "at zero. The symbols _sbss, _ebss mark the range.\n\n"
        "3. Call main(). If main returns, we spin in an infinite loop.\n\n"
        "This is the ENTIRE C runtime for our RTOS. No crt0.o, no __libc_init."
    )

    pdf.section("Weak Aliases")
    pdf.body(
        "Each exception handler is declared as a 'weak alias' of Default_Handler. "
        "This means: if we define PendSV_Handler elsewhere (we do, in assembly), "
        "the linker uses our version. If we don't define one (like DebugMon_Handler), "
        "it falls back to Default_Handler which just spins. This is a clean pattern "
        "for handling interrupts you haven't implemented yet."
    )

    # ── CH 6: Linker Script ─────────────────────────────────────────
    pdf.add_page()
    pdf.chapter_title("6. The Linker Script")

    pdf.body(
        "The linker script (linker.ld) tells the linker WHERE to place each section "
        "of our program in the target's memory. This is critical in embedded "
        "because there is no OS to handle memory mapping.\n\n"
        "Our memory map:"
    )

    pdf.code(
        "MEMORY {\n"
        "    FLASH (rx)  : ORIGIN = 0x00000000, LENGTH = 256K\n"
        "    SRAM  (rwx) : ORIGIN = 0x20000000, LENGTH = 64K\n"
        "}"
    )

    pdf.body(
        "_estack = top of SRAM (0x20010000). This is the initial MSP value.\n\n"
        "Section placement:\n"
        "- .isr_vector -> FLASH (must be at address 0x00000000)\n"
        "- .text (code) -> FLASH\n"
        "- .rodata (const data) -> FLASH\n"
        "- .data (initialized globals) -> SRAM, but loaded from FLASH\n"
        "  The 'AT> FLASH' directive stores .data in flash; Reset_Handler copies it.\n"
        "- .bss (uninitialized globals) -> SRAM\n"
        "- ._heap (our heap pool) -> SRAM\n\n"
        "The symbols (_sidata, _sdata, _edata, _sbss, _ebss) are generated by the "
        "linker and used by startup.c to perform the copy and zero operations."
    )

    # ── CH 7: Context Switching ─────────────────────────────────────
    pdf.add_page()
    pdf.chapter_title("7. Context Switching Deep Dive")

    pdf.body(
        "Context switching is the CORE mechanism of any RTOS. It saves one task's "
        "CPU state and restores another's, giving the illusion that multiple tasks "
        "run simultaneously on a single core."
    )

    pdf.section("What is a Task's Context?")
    pdf.body(
        "A task's context is the complete set of CPU registers that define its "
        "execution state:\n\n"
        "- R0-R12: general-purpose registers\n"
        "- SP (R13): stack pointer -- where the task's data lives\n"
        "- LR (R14): link register -- where to return after a function call\n"
        "- PC (R15): program counter -- the next instruction to execute\n"
        "- xPSR: program status register (flags, thumb state, etc.)\n\n"
        "To switch tasks, we save all of these to the current task's stack, "
        "then load all of them from the next task's stack."
    )

    pdf.section("How Cortex-M3 Helps Us")
    pdf.body(
        "The Cortex-M3 hardware does HALF the work for us. When an exception fires:\n\n"
        "Hardware automatically saves to the current stack:\n"
        "  R0, R1, R2, R3, R12, LR, PC, xPSR  (8 registers)\n\n"
        "We manually save in our PendSV handler:\n"
        "  R4, R5, R6, R7, R8, R9, R10, R11    (8 registers)\n\n"
        "On exception return, hardware automatically restores the first 8. "
        "We restore R4-R11 manually before returning."
    )

    pdf.section("The PendSV Handler (context_switch.s)")
    pdf.body(
        "PendSV (Pendable Supervisor Call) is an exception we trigger by software. "
        "We set it to the LOWEST priority so it runs only after all other ISRs are "
        "done. This is critical -- a context switch during a higher-priority ISR "
        "would corrupt state.\n\n"
        "Step by step:"
    )

    pdf.code(
        "PendSV_Handler:\n"
        "  CPSID I               ; Disable interrupts\n"
        "  MRS   R0, PSP         ; R0 = current task's stack pointer\n"
        "  CBZ   R0, skip_save   ; If PSP=0, this is the first switch\n"
        "\n"
        "  ; Save R4-R11 onto the current task's stack\n"
        "  STMDB R0!, {R4-R11}   ; Push R4-R11, decrement R0\n"
        "\n"
        "  ; Save updated stack pointer to TCB\n"
        "  LDR R1, =current_task\n"
        "  LDR R1, [R1]          ; R1 = pointer to current TCB\n"
        "  STR R0, [R1, #0]      ; TCB.stack_ptr = R0\n"
        "\n"
        "skip_save:\n"
        "  BL  os_schedule        ; C function picks next task\n"
        "\n"
        "  ; Load next task's stack pointer from TCB\n"
        "  LDR R1, =current_task\n"
        "  LDR R1, [R1]          ; R1 = pointer to next TCB\n"
        "  LDR R0, [R1, #0]      ; R0 = next TCB.stack_ptr\n"
        "\n"
        "  ; Restore R4-R11 from next task's stack\n"
        "  LDMIA R0!, {R4-R11}   ; Pop R4-R11, increment R0\n"
        "\n"
        "  MSR PSP, R0           ; Set PSP to next task's stack\n"
        "  ORR LR, LR, #0x04    ; EXC_RETURN bit 2 = use PSP\n"
        "  CPSIE I               ; Re-enable interrupts\n"
        "  BX  LR                ; Exception return -> next task"
    )

    pdf.section("Stack Frame Layout")
    pdf.body(
        "After context save, a task's stack looks like this (low addr at top):\n\n"
        "stack_ptr -->  R4\n"
        "               R5\n"
        "               R6\n"
        "               R7\n"
        "               R8\n"
        "               R9\n"
        "               R10\n"
        "               R11\n"
        "PSP ---------> R0    (hardware-saved frame)\n"
        "               R1\n"
        "               R2\n"
        "               R3\n"
        "               R12\n"
        "               LR\n"
        "               PC    (return address)\n"
        "               xPSR\n\n"
        "When we create a new task, we pre-fill this exact layout so the first "
        "context switch loads a valid frame and branches to the task function."
    )

    # ── CH 8: The Scheduler ─────────────────────────────────────────
    pdf.add_page()
    pdf.chapter_title("8. The Scheduler")

    pdf.section("Priority-Based Preemptive Scheduling")
    pdf.body(
        "Our scheduler uses priority-based preemptive scheduling with round-robin "
        "within the same priority level.\n\n"
        "How it works:\n"
        "1. There are MAX_PRIORITY_LEVELS (8) priority levels, 0 (lowest) to 7 (highest).\n"
        "2. Each level has a FIFO linked list of ready tasks (the 'ready queue').\n"
        "3. os_schedule() scans from the highest priority down and picks the first "
        "task from the first non-empty queue.\n"
        "4. Within the same priority, tasks rotate in FIFO order (round-robin).\n\n"
        "'Preemptive' means: if a high-priority task becomes ready (e.g., its delay "
        "expires), it immediately preempts a lower-priority running task. The SysTick "
        "ISR checks this every 1 ms."
    )

    pdf.section("Ready Queue Operations")
    pdf.body(
        "os_ready_queue_add(task): appends the task to the tail of its priority's list.\n"
        "os_ready_queue_remove(task): removes the task from its priority's list.\n\n"
        "The remove function uses the pointer-to-pointer pattern for clean "
        "singly-linked-list removal without special-casing the head."
    )

    pdf.section("os_schedule() Logic")
    pdf.code(
        "void os_schedule(void) {\n"
        "    // If current task was preempted (still RUNNING),\n"
        "    // put it back in the ready queue\n"
        "    if (current_task && current_task->state == RUNNING) {\n"
        "        current_task->state = READY;\n"
        "        os_ready_queue_add(current_task);\n"
        "    }\n"
        "\n"
        "    // Scan from highest to lowest priority\n"
        "    for (i = MAX_PRIO-1; i >= 0; i--) {\n"
        "        if (ready_queue[i] != NULL) {\n"
        "            current_task = ready_queue[i];\n"
        "            ready_queue[i] = current_task->next;\n"
        "            current_task->state = RUNNING;\n"
        "            return;\n"
        "        }\n"
        "    }\n"
        "    // idle task at priority 0 guarantees this never fails\n"
        "}"
    )

    pdf.section("The Idle Task")
    pdf.body(
        "The idle task runs at priority 0 and is always in the ready queue. It "
        "executes the WFI (Wait For Interrupt) instruction, which puts the CPU "
        "into a low-power sleep until the next interrupt. This prevents the "
        "scheduler from ever having an empty queue."
    )

    # ── CH 9: Task Management ───────────────────────────────────────
    pdf.add_page()
    pdf.chapter_title("9. Task Management")

    pdf.section("Task Control Block (TCB)")
    pdf.body(
        "Each task is described by a TCB (Task Control Block) struct:\n\n"
        "- stack_ptr: saved stack pointer (MUST be the first field -- assembly depends on this)\n"
        "- stack[STACK_SIZE]: the task's private stack (2048 bytes by default)\n"
        "- name: human-readable name for the debug shell\n"
        "- priority / base_priority: current and original priority (for inheritance)\n"
        "- state: READY, RUNNING, BLOCKED, SUSPENDED, or TERMINATED\n"
        "- delay_ticks: countdown for os_task_delay()\n"
        "- id: index into the task_pool array\n"
        "- next: linked-list pointer for ready/wait queues"
    )

    pdf.section("Creating a Task")
    pdf.body(
        "os_task_create() does the following:\n\n"
        "1. Finds a free slot in task_pool (state == TERMINATED).\n"
        "2. Initializes the TCB fields (name, priority, state = READY).\n"
        "3. Sets up the initial stack frame so it looks like the task was "
        "interrupted mid-execution. The PC slot points to the task function, "
        "R0 holds the argument, and LR points to a cleanup handler.\n"
        "4. Adds the task to the appropriate ready queue.\n"
        "5. Returns the task ID (slot index)."
    )

    pdf.section("Stack Initialization")
    pdf.body(
        "The key insight: we pre-fill the stack to mimic what the hardware would "
        "save during an exception. When PendSV restores this fake frame, the CPU "
        "thinks it is returning from an interrupt INTO the task function.\n\n"
        "The xPSR is set to 0x01000000 (Thumb bit set -- Cortex-M3 ONLY runs Thumb "
        "code). The LR slot in the frame points to task_return_handler(), which "
        "calls os_task_delete() if a task's function returns."
    )

    pdf.section("Delays and Yielding")
    pdf.body(
        "os_task_delay(ticks): Sets delay_ticks and changes state to BLOCKED. "
        "The SysTick ISR decrements this each tick and wakes the task when it "
        "reaches zero.\n\n"
        "os_task_yield(): Simply triggers PendSV. The current task stays RUNNING, "
        "so os_schedule() puts it back in the ready queue and picks the next one. "
        "This allows round-robin among equal-priority tasks."
    )

    # ── CH 10: SysTick ──────────────────────────────────────────────
    pdf.add_page()
    pdf.chapter_title("10. SysTick Timer & System Clock")

    pdf.body(
        "The SysTick timer is a 24-bit countdown timer built into every Cortex-M3. "
        "We configure it to fire every 1 ms (1000 Hz) as the RTOS heartbeat."
    )

    pdf.section("Configuration")
    pdf.code(
        "void systick_init(void) {\n"
        "    SYST_RVR = (50000000 / 1000) - 1;  // 49999\n"
        "    SYST_CVR = 0;\n"
        "    SYST_CSR = CLKSOURCE | TICKINT | ENABLE;\n"
        "}"
    )

    pdf.body(
        "At 50 MHz, the counter counts down from 49999 to 0, then reloads and "
        "fires the SysTick exception. 50000 clock cycles = 1 ms."
    )

    pdf.section("SysTick Handler")
    pdf.body(
        "Every tick (1 ms), the handler:\n"
        "1. Increments the global tick counter.\n"
        "2. Scans all tasks: if BLOCKED with delay_ticks > 0, decrement it. "
        "When it hits 0, set state to READY and add to the ready queue.\n"
        "3. Triggers PendSV to potentially switch to a newly-woken higher-priority task.\n\n"
        "This is why PendSV must be the LOWEST priority exception -- it runs after "
        "SysTick finishes updating the ready queues."
    )

    # ── CH 11: Mutexes ──────────────────────────────────────────────
    pdf.add_page()
    pdf.chapter_title("11. Mutexes & Priority Inheritance")

    pdf.section("The Problem: Shared Resources")
    pdf.body(
        "When two tasks access the same resource (like UART), they can corrupt each "
        "other's output. A mutex (mutual exclusion) ensures only one task holds the "
        "resource at a time."
    )

    pdf.section("Priority Inversion")
    pdf.body(
        "Without priority inheritance, a dangerous scenario called 'priority inversion' "
        "can occur:\n\n"
        "1. Low-priority task L acquires mutex M.\n"
        "2. High-priority task H tries to acquire M, blocks.\n"
        "3. Medium-priority task M (no relation to the mutex) preempts L.\n"
        "4. H is now blocked by M, even though H has higher priority!\n\n"
        "The Mars Pathfinder spacecraft experienced this exact bug in 1997, causing "
        "repeated system resets."
    )

    pdf.section("Our Solution: Priority Inheritance")
    pdf.body(
        "When task H blocks on a mutex held by task L:\n"
        "1. L's priority is temporarily boosted to H's priority.\n"
        "2. L runs at the boosted priority, finishes its critical section faster.\n"
        "3. When L releases the mutex, its priority reverts to base_priority.\n"
        "4. H acquires the mutex and runs.\n\n"
        "This prevents medium-priority tasks from preempting L while H is waiting."
    )

    pdf.section("Implementation")
    pdf.body(
        "mutex_lock():\n"
        "- If unlocked: set lock=1, owner=current_task. Done.\n"
        "- If locked: boost owner's priority if needed, block current task, "
        "add to wait_queue, trigger PendSV.\n\n"
        "mutex_unlock():\n"
        "- Restore owner's base_priority.\n"
        "- If wait_queue is non-empty: transfer ownership to the first waiter, "
        "wake it up. Otherwise: set lock=0, owner=NULL."
    )

    # ── CH 12: Semaphores ───────────────────────────────────────────
    pdf.add_page()
    pdf.chapter_title("12. Semaphores")

    pdf.body(
        "A semaphore is a signaling mechanism with a counter. Unlike a mutex "
        "(which is binary: locked/unlocked), a counting semaphore allows up to N "
        "concurrent accesses.\n\n"
        "Use cases:\n"
        "- Limiting access to a pool of N identical resources.\n"
        "- Producer/consumer synchronization (producer posts, consumer waits).\n"
        "- Event notification (one task signals another)."
    )

    pdf.section("API")
    pdf.body(
        "sem_init(s, initial_count): Create with an initial count.\n"
        "sem_wait(s): If count > 0, decrement and continue. If count == 0, block.\n"
        "sem_post(s): If tasks are waiting, wake one. Otherwise increment count.\n\n"
        "Key difference from mutex: a semaphore has no owner. Any task can post, "
        "and there is no priority inheritance. Use mutexes for mutual exclusion, "
        "semaphores for signaling/counting."
    )

    # ── CH 13: Message Queues ───────────────────────────────────────
    pdf.add_page()
    pdf.chapter_title("13. Message Queues (IPC)")

    pdf.body(
        "Message queues allow tasks to exchange fixed-size messages through a "
        "shared buffer. They provide inter-process communication (IPC) with "
        "decoupling -- the producer and consumer don't need to run at the same time."
    )

    pdf.section("Ring Buffer Design")
    pdf.body(
        "Our message queue uses a circular (ring) buffer:\n\n"
        "- buffer[]: array of MQ_MAX_MSGS slots, each MQ_MSG_SIZE bytes.\n"
        "- head: index of the next message to READ.\n"
        "- tail: index of the next slot to WRITE.\n"
        "- count: number of messages currently in the queue.\n\n"
        "When tail reaches the end, it wraps to 0. Same for head. This gives O(1) "
        "enqueue and dequeue with no shifting."
    )

    pdf.section("Blocking Behavior")
    pdf.body(
        "mq_send(): If the queue is full, the sender blocks (added to send_wait queue). "
        "When a receiver takes a message, it wakes a blocked sender.\n\n"
        "mq_recv(): If the queue is empty, the receiver blocks (added to recv_wait queue). "
        "When a sender adds a message, it wakes a blocked receiver.\n\n"
        "This is the classic bounded producer-consumer pattern. Our demo uses it to "
        "show the producer sending messages that the consumer processes."
    )

    # ── CH 14: Heap Allocator ───────────────────────────────────────
    pdf.add_page()
    pdf.chapter_title("14. Heap Allocator")

    pdf.body(
        "Our RTOS provides os_malloc() and os_free() for dynamic memory allocation. "
        "We implement a first-fit allocator with block splitting and forward coalescing."
    )

    pdf.section("How It Works")
    pdf.body(
        "The heap is a statically-allocated array (16 KB by default). It is divided "
        "into blocks, each preceded by a header:\n\n"
        "  struct block { size, used, *next };\n\n"
        "os_malloc(size):\n"
        "1. Walk the linked list of blocks.\n"
        "2. Find the first FREE block large enough (first-fit).\n"
        "3. If the block is much larger than needed, split it: carve off the "
        "requested size and create a new free block with the remainder.\n"
        "4. Mark the block as used and return a pointer past the header.\n\n"
        "os_free(ptr):\n"
        "1. Back up to the block header (ptr - sizeof(header)).\n"
        "2. Mark the block as free.\n"
        "3. If the NEXT block is also free, merge them (coalesce). This reduces "
        "fragmentation."
    )

    pdf.section("Limitations")
    pdf.body(
        "- First-fit can cause fragmentation over time.\n"
        "- We only coalesce forward (with the next block), not backward.\n"
        "- No alignment beyond 4 bytes.\n"
        "- Not thread-safe in the general case (we use __disable_irq as a crude lock).\n\n"
        "For a production RTOS, you would use a more sophisticated allocator like "
        "TLSF (Two-Level Segregated Fit) which gives O(1) allocation."
    )

    # ── CH 15: UART Driver ──────────────────────────────────────────
    pdf.add_page()
    pdf.chapter_title("15. UART Driver & printf")

    pdf.body(
        "The UART (Universal Asynchronous Receiver/Transmitter) is our only I/O "
        "channel. QEMU maps UART0 to the terminal, so uart_putc('A') makes 'A' "
        "appear in your console."
    )

    pdf.section("LM3S6965 UART Registers")
    pdf.body(
        "UART0 base address: 0x4000C000\n\n"
        "UARTDR  (+0x000): Data Register -- write a byte to transmit, read to receive.\n"
        "UARTFR  (+0x018): Flag Register --\n"
        "  Bit 4 (RXFE): 1 = receive FIFO empty (nothing to read)\n"
        "  Bit 5 (TXFF): 1 = transmit FIFO full (can't write yet)\n"
        "UARTIBRD (+0x024): Integer baud rate divisor\n"
        "UARTFBRD (+0x028): Fractional baud rate divisor\n"
        "UARTLCRH (+0x02C): Line control (word length, parity, FIFOs)\n"
        "UARTCTL  (+0x030): Control (enable UART, TX, RX)"
    )

    pdf.section("Baud Rate Calculation")
    pdf.body(
        "For 115200 baud at 50 MHz:\n"
        "  Divisor = 50000000 / (16 * 115200) = 27.1267\n"
        "  Integer part (IBRD) = 27\n"
        "  Fractional part (FBRD) = round(0.1267 * 64) = 8\n\n"
        "On QEMU, the baud rate doesn't actually matter (it's virtual), but "
        "setting it correctly is good practice and makes the code portable to "
        "real hardware."
    )

    pdf.section("Minimal printf")
    pdf.body(
        "We implement uart_printf() with support for:\n"
        "  %d -- signed integer\n"
        "  %u -- unsigned integer\n"
        "  %x -- hexadecimal (with 0x prefix)\n"
        "  %s -- string\n"
        "  %c -- character\n"
        "  %% -- literal percent\n\n"
        "This uses <stdarg.h> (va_list) which is provided by the compiler, not "
        "the C library, so it works in our freestanding environment."
    )

    # ── CH 16: Debug Shell ──────────────────────────────────────────
    pdf.add_page()
    pdf.chapter_title("16. Debug Shell")

    pdf.body(
        "The shell runs as a regular task (priority 4 -- highest user priority) and "
        "provides an interactive command-line interface over UART. It reads characters "
        "one by one, builds a command line, and executes it when Enter is pressed."
    )

    pdf.section("Commands")
    pdf.body(
        "ps    -- Lists all active tasks with ID, name, priority, state, stack usage, "
        "and fault count.\n\n"
        "top   -- Per-task CPU usage: shows percentage of total ticks each task has "
        "consumed and context-switch counts.\n\n"
        "free  -- Shows heap memory usage: bytes used, bytes free, total pool size.\n\n"
        "uptime -- Displays how long the system has been running (minutes, seconds, "
        "raw tick count).\n\n"
        "trace -- Dumps the last 32 events from the KTrace ring buffer with "
        "timestamps, event types, and task IDs.\n\n"
        "fault -- Shows the fault history log: every stack overflow and HardFault "
        "with timestamp, faulting PC/LR, and task ID.\n\n"
        "crash -- Triggers a deliberate stack overflow on the 'stress' task to "
        "demonstrate the auto-recovery system.\n\n"
        "version -- Kernel version and target info.\n\n"
        "help -- Prints the command list."
    )

    pdf.section("Stack Usage Estimation")
    pdf.body(
        "The 'ps' command estimates stack usage by counting how many words at the "
        "bottom of the stack array are still at their initial value. Since the stack "
        "grows downward from the top of the array, untouched words at the bottom "
        "indicate unused space. The canary words at the very bottom are excluded "
        "from this count."
    )

    # ── CH 17: Building & Running ───────────────────────────────────
    pdf.add_page()
    pdf.chapter_title("17. Building & Running")

    pdf.section("Build")
    pdf.code(
        "make          # Compile everything, produce nexus-rtos.elf and nexus-rtos.bin"
    )

    pdf.body(
        "What happens during the build:\n"
        "1. Each .c file is compiled with arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb.\n"
        "   -ffreestanding: no standard library assumptions.\n"
        "   -nostdlib: don't link libc.\n"
        "2. context_switch.s is assembled with arm-none-eabi-gcc.\n"
        "3. All .o files are linked with our linker.ld script.\n"
        "4. objcopy extracts a raw binary (nexus-rtos.bin) from the ELF.\n"
        "5. size prints the final memory usage."
    )

    pdf.section("Run on QEMU")
    pdf.code(
        "make run\n"
        "# or directly:\n"
        "qemu-system-arm -M lm3s6965evb -nographic -kernel nexus-rtos.elf"
    )

    pdf.body(
        "QEMU loads the ELF, sets up the emulated LM3S6965, and starts executing "
        "from the Reset_Handler. UART0 output appears in your terminal. You can "
        "type commands to the shell.\n\n"
        "To quit: press Ctrl+A, then X."
    )

    pdf.section("Windows Notes")
    pdf.body(
        "On Windows, the recommended setup is MSYS2 with MinGW64. Open an MSYS2 "
        "MinGW64 terminal and install the tools:\n\n"
        "  pacman -S mingw-w64-x86_64-arm-none-eabi-gcc\n"
        "  pacman -S mingw-w64-x86_64-qemu-system-arm\n"
        "  pacman -S make\n\n"
        "Then just 'make' and 'make run' from the project directory.\n\n"
        "Alternatively, you can use WSL (Windows Subsystem for Linux) with Ubuntu "
        "and follow the Ubuntu installation instructions. WSL works seamlessly."
    )

    # ── CH 18: Debugging ────────────────────────────────────────────
    pdf.add_page()
    pdf.chapter_title("18. Debugging with GDB")

    pdf.section("Starting a Debug Session")
    pdf.code(
        "# Terminal 1: Start QEMU paused\n"
        "make debug\n"
        "\n"
        "# Terminal 2: Connect GDB\n"
        "arm-none-eabi-gdb nexus-rtos.elf -x .gdbinit"
    )

    pdf.body(
        "The .gdbinit script automatically connects to QEMU on port 3333, "
        "loads the binary, and sets breakpoints on main and HardFault_Handler."
    )

    pdf.section("Essential GDB Commands")
    pdf.body(
        "Navigation:\n"
        "  continue (c)       -- run until next breakpoint\n"
        "  step (s)           -- step into functions\n"
        "  next (n)           -- step over functions\n"
        "  finish             -- run until current function returns\n\n"
        "Breakpoints:\n"
        "  break SysTick_Handler  -- break on every tick\n"
        "  break PendSV_Handler   -- break on every context switch\n"
        "  break mutex_lock       -- break when any task acquires a mutex\n\n"
        "Inspection:\n"
        "  print current_task->name     -- which task is running?\n"
        "  print current_task->priority -- its priority level\n"
        "  print task_pool[0]           -- inspect task 0's full TCB\n"
        "  x/16x current_task->stack_ptr -- hex dump the stack\n"
        "  info registers               -- all CPU registers\n\n"
        "Watchpoints:\n"
        "  watch current_task  -- break whenever the running task changes"
    )

    # ── CH 19: KTrace ──────────────────────────────────────────────
    pdf.add_page()
    pdf.chapter_title("19. KTrace: Kernel Event Tracer")

    pdf.body(
        "KTrace is a lightweight kernel event tracer that records what the kernel "
        "does in real time. Production RTOS kernels like FreeRTOS (with Tracealyzer) "
        "and Zephyr (with its tracing subsystem) have similar features. Building one "
        "from scratch demonstrates observability design."
    )

    pdf.section("Design Goals")
    pdf.body(
        "1. Zero-allocation: uses a statically-allocated ring buffer.\n"
        "2. Lockless: safe to call from ISR context without disabling interrupts "
        "(the ring buffer head is advanced atomically for a single producer).\n"
        "3. Low overhead: each event is 8 bytes (timestamp, type, task_id, extra).\n"
        "4. Configurable: TRACE_BUF_SIZE in os_config.h controls how many events "
        "are retained (default 256)."
    )

    pdf.section("Event Types")
    pdf.body(
        "EVT_TASK_SWITCH  -- Context switch: which task is now running\n"
        "EVT_TASK_CREATE  -- New task created: extra = priority\n"
        "EVT_TASK_DELETE  -- Task terminated\n"
        "EVT_TASK_DELAY   -- Task called os_task_delay(): extra = ticks\n"
        "EVT_TASK_WAKE    -- Task woken from delay\n"
        "EVT_MUTEX_LOCK   -- Mutex acquired\n"
        "EVT_MUTEX_UNLOCK -- Mutex released\n"
        "EVT_MUTEX_BLOCK  -- Task blocked on mutex: extra = owner task ID\n"
        "EVT_FAULT        -- HardFault occurred: extra = low 16 bits of faulting PC\n"
        "EVT_STACK_OVF    -- Stack overflow detected\n"
        "EVT_TASK_RESTART -- Faulted task auto-restarted: extra = crash count"
    )

    pdf.section("Ring Buffer Implementation")
    pdf.code(
        "typedef struct {\n"
        "    uint32_t timestamp;\n"
        "    uint8_t  type;\n"
        "    uint8_t  task_id;\n"
        "    uint16_t extra;\n"
        "} trace_event_t;  // 8 bytes\n"
        "\n"
        "static trace_event_t trace_buf[TRACE_BUF_SIZE];\n"
        "static volatile uint32_t trace_head;\n"
        "\n"
        "void ktrace_log(uint8_t type, uint8_t task_id, uint16_t extra) {\n"
        "    trace_event_t *evt = &trace_buf[trace_head];\n"
        "    evt->timestamp = os_get_ticks();\n"
        "    evt->type      = type;\n"
        "    evt->task_id   = task_id;\n"
        "    evt->extra     = extra;\n"
        "    trace_head = (trace_head + 1) % TRACE_BUF_SIZE;\n"
        "}"
    )

    pdf.section("How to Use")
    pdf.body(
        "The trace is automatically populated by the kernel. Use the shell 'trace' "
        "command to dump the last 32 events. This is invaluable for debugging:\n\n"
        "- See the exact sequence of context switches\n"
        "- Find which task was running when a fault occurred\n"
        "- Diagnose priority inversion by checking mutex block/unlock patterns\n"
        "- Measure scheduling latency by comparing event timestamps"
    )

    # ── CH 20: Fault Isolation ─────────────────────────────────────
    pdf.add_page()
    pdf.chapter_title("20. Fault Isolation & Auto-Recovery")

    pdf.body(
        "Most teaching RTOS projects crash on any fault. NexusRTOS implements a "
        "fault isolation pipeline that detects, diagnoses, and recovers from "
        "task-level faults -- a key concept in safety-critical systems."
    )

    pdf.section("Stack Canary Painting")
    pdf.body(
        "On task creation, the bottom 4 words of each task's stack are painted with "
        "the canary value 0xDEADBEEF. Since the ARM Cortex-M stack grows downward "
        "(from high addresses to low), a stack overflow will overwrite these canary "
        "words first.\n\n"
        "This is the same technique used by GCC's -fstack-protector and production "
        "RTOS kernels."
    )

    pdf.code(
        "// In os_task_create():\n"
        "for (int i = 0; i < STACK_CANARY_WORDS; i++)\n"
        "    tcb->stack[i] = STACK_CANARY;  // 0xDEADBEEF"
    )

    pdf.section("Round-Robin Canary Scanning")
    pdf.body(
        "The SysTick ISR checks ONE task's canaries per tick, cycling through all "
        "tasks. With 16 task slots at 1000 Hz, every task is checked every 16 ms.\n\n"
        "This amortized approach adds near-zero overhead per tick (checking 4 words), "
        "rather than scanning all tasks every tick."
    )

    pdf.section("HardFault Handler")
    pdf.body(
        "For faults the canary scan cannot catch (e.g., dereferencing a bad pointer), "
        "the HardFault handler extracts the exception frame and calls a C handler:\n\n"
        "1. Assembly wrapper determines whether MSP or PSP was in use (checks bit 2 "
        "of EXC_RETURN in LR).\n"
        "2. Passes the stacked frame pointer and EXC_RETURN to HardFault_Handler_C.\n"
        "3. The C handler logs diagnostics: faulting PC, LR, task name, task ID.\n"
        "4. Records the fault in the fault log and KTrace.\n"
        "5. If the task is marked 'restartable', rebuilds its stack frame and re-adds "
        "it to the ready queue."
    )

    pdf.code(
        "HardFault_Handler:      ; in context_switch.s\n"
        "    TST   LR, #4       ; Check EXC_RETURN bit 2\n"
        "    ITE   EQ\n"
        "    MRSEQ R0, MSP      ; If 0: handler was using MSP\n"
        "    MRSNE R0, PSP      ; If 1: thread was using PSP\n"
        "    MOV   R1, LR       ; Pass EXC_RETURN\n"
        "    B     HardFault_Handler_C  ; Tail-call to C handler"
    )

    pdf.section("Auto-Recovery")
    pdf.body(
        "Tasks created with .restartable = 1 are automatically restarted after a fault:\n\n"
        "1. The TCB saves the original entry function and argument.\n"
        "2. On fault, the stack is re-initialized to a fresh exception frame pointing "
        "to the original entry function.\n"
        "3. Stack canaries are re-painted.\n"
        "4. The task is added back to the ready queue.\n"
        "5. A fault_count is incremented for monitoring.\n\n"
        "The 'crash' shell command triggers a deliberate stack overflow on the 'stress' "
        "task to demonstrate this. The 'fault' command shows the fault history."
    )

    # ── CH 21: CPU Profiler ────────────────────────────────────────
    pdf.add_page()
    pdf.chapter_title("21. Per-Task CPU Profiler")

    pdf.body(
        "The CPU profiler tracks how much execution time each task consumes. This "
        "is essential for performance analysis in real-time systems where meeting "
        "deadlines depends on understanding CPU budgets."
    )

    pdf.section("Implementation")
    pdf.body(
        "The scheduler records timestamps on every context switch:\n\n"
        "1. When a task is switched OUT, its run_ticks is incremented by "
        "(current_tick - last_switched_in).\n"
        "2. When a task is switched IN, last_switched_in is set to the current tick.\n"
        "3. switch_count is incremented each time a task is selected.\n\n"
        "This gives us:\n"
        "- CPU%: (run_ticks / uptime_ticks) * 100 for each task\n"
        "- Switch frequency: how often the task is scheduled\n\n"
        "The 'top' shell command displays this in a table."
    )

    pdf.section("What You Can Learn From It")
    pdf.body(
        "- Which tasks dominate CPU time (is the idle task getting enough cycles?)\n"
        "- Whether a task is switching too frequently (high overhead)\n"
        "- CPU utilization: if the idle task gets < 20%, the system is heavily loaded\n"
        "- Whether priority assignments match actual workload importance\n\n"
        "Understanding per-task CPU usage is essential for reasoning about "
        "real-time guarantees and spotting tasks that consume more time than expected."
    )

    # ── CH 22: Extending ────────────────────────────────────────────
    pdf.add_page()
    pdf.chapter_title("22. Extending the Project")

    pdf.body("Ideas for further development:")

    extensions = [
        ("Tickless Idle Mode",
         "Instead of waking every 1 ms, calculate when the next task will be ready "
         "and sleep until then. This drastically reduces power consumption."),
        ("MPU (Memory Protection Unit)",
         "Use the Cortex-M3's MPU to isolate each task's stack. A stack overflow "
         "triggers a MemManage exception instead of silently corrupting another task."),
        ("Interrupt-Driven UART",
         "Replace polling with RX/TX interrupts and ring buffers. This frees the "
         "CPU while waiting for characters."),
        ("Priority Queue with Deadline",
         "Implement EDF (Earliest Deadline First) scheduling alongside fixed-priority."),
        ("Software Timers",
         "Allow tasks to register callbacks that fire after a delay without "
         "creating a full task."),
        ("Mutex with Recursive Locking",
         "Allow the same task to lock a mutex multiple times without deadlocking."),
        ("CPU Usage Tracking",
         "Measure how much time each task spends running vs. idle. Display in the "
         "shell with a 'top' command."),
        ("Port to Real Hardware",
         "Flash this onto a real STM32 (Cortex-M3/M4) board. The code is nearly "
         "identical -- just update the UART and clock registers."),
    ]
    for title, desc in extensions:
        pdf.set_font("Helvetica", "B", 10)
        pdf.set_text_color(30, 60, 120)
        pdf.cell(0, 7, title, new_x="LMARGIN", new_y="NEXT")
        pdf.set_font("Helvetica", "", 10)
        pdf.set_text_color(30, 30, 30)
        pdf.multi_cell(0, 5.5, desc)
        pdf.ln(3)

    # ── CH 23: Interview Q&A ────────────────────────────────────────
    pdf.add_page()
    pdf.chapter_title("23. Common Questions & Answers")

    pdf.body(
        "These are common questions about RTOS internals and the design decisions "
        "behind NexusRTOS:"
    )

    qas = [
        ("Q: Why did you use PendSV for context switching instead of SysTick?",
         "A: PendSV can be set to the lowest exception priority, ensuring the "
         "context switch happens AFTER all other ISRs complete. If we switched "
         "in SysTick, a higher-priority interrupt arriving mid-switch would see "
         "inconsistent state. PendSV is 'pendable' -- we set a flag and it fires "
         "when nothing else is pending."),
        ("Q: Explain priority inheritance in your mutex implementation.",
         "A: When a high-priority task H blocks on a mutex held by low-priority "
         "task L, I temporarily boost L's priority to H's level. This prevents "
         "priority inversion, where a medium-priority task M could preempt L and "
         "indirectly block H. When L releases the mutex, its priority reverts. "
         "This is the same protocol that was patched into Mars Pathfinder's VxWorks."),
        ("Q: How does your scheduler achieve O(1) task selection?",
         "A: Each priority level has its own ready queue (linked list). The "
         "scheduler scans from the highest priority downward -- with 8 levels, "
         "this is a constant-time scan. Within a level, tasks are dequeued from "
         "the head (FIFO), giving round-robin. A bitmap optimization could make "
         "this truly O(1) regardless of the number of priority levels."),
        ("Q: What happens when a task's stack overflows?",
         "A: NexusRTOS detects it through stack canaries -- four words of 0xDEADBEEF "
         "painted at the bottom of each stack. The SysTick ISR scans one task per "
         "tick in round-robin, so every task is checked within 16 ms. On detection, "
         "the fault handler logs the event, terminates the task, and if it is marked "
         "restartable, re-initializes its stack and re-adds it to the ready queue. "
         "HardFaults are also caught via an assembly wrapper that extracts the "
         "exception frame and delegates to a C handler for diagnostics."),
        ("Q: Why not use the standard C library?",
         "A: The standard library assumes an underlying OS for malloc, printf, "
         "file I/O, etc. We ARE the OS, so those assumptions don't hold. We "
         "use -ffreestanding -nostdlib and provide our own printf (via UART) "
         "and malloc (our heap allocator). The only compiler-provided headers "
         "we use are stdint.h, stddef.h, and stdarg.h."),
        ("Q: How would you port this to real hardware?",
         "A: The Cortex-M3 core is identical across vendors -- the context switch, "
         "scheduler, and synchronization code work unchanged. You'd update: "
         "(1) the linker script for the new chip's memory map, (2) the UART "
         "register addresses and clock setup, and (3) any vendor-specific "
         "peripheral initialization. I'd start with an STM32F103 (Blue Pill board, "
         "about $2)."),
        ("Q: How does your kernel event tracer work and why did you build it?",
         "A: KTrace is a lockless ring buffer of 8-byte events that logs context "
         "switches, mutex operations, task lifecycle, and faults with timestamps. "
         "I built it because observability is critical in real-time systems -- when "
         "debugging a priority inversion or missed deadline, you need a trace of "
         "what the kernel did, not just what the application printed. It is modeled "
         "after FreeRTOS Tracealyzer and Zephyr's tracing subsystem."),
        ("Q: How does your auto-recovery mechanism work? Is it safe?",
         "A: When a fault is detected, the handler kills the faulting task, rebuilds "
         "its stack frame pointing to the original entry function, re-paints the "
         "canaries, and adds it back to the ready queue. This is safe because each "
         "task has its own stack -- we do not touch other tasks' memory. The fault "
         "count tracks how many times a task has crashed, which could be used to "
         "implement a 'three strikes' policy. In a production system, you would "
         "combine this with the MPU for stronger isolation."),
        ("Q: How do you measure per-task CPU usage?",
         "A: The scheduler timestamps each context switch. When a task is switched "
         "out, run_ticks += (now - last_switched_in). The 'top' command divides "
         "each task's run_ticks by total uptime to get a percentage. This is the "
         "same approach used by Linux's /proc/stat, just at the RTOS scale."),
    ]
    for q, a in qas:
        pdf.set_font("Helvetica", "B", 10)
        pdf.set_text_color(30, 60, 120)
        pdf.multi_cell(0, 5.5, q)
        pdf.ln(1)
        pdf.set_font("Helvetica", "", 10)
        pdf.set_text_color(30, 30, 30)
        pdf.multi_cell(0, 5.5, a)
        pdf.ln(5)

    # ── Save ────────────────────────────────────────────────────────
    import os
    out = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "notes.pdf")
    pdf.output(out)
    print(f"Generated: {os.path.abspath(out)}")


if __name__ == "__main__":
    build_pdf()
