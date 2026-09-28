target remote localhost:3333
load nexus-rtos.elf
break main
break HardFault_Handler
continue
