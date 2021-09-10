#include "drivers/serial_port/serial.c"
#include "memory/segmentation/memory_segments.h"
#include "drivers/interrupts/interrupts.h"
#include "multiboot.h"
#include "utils/type.h"
#include "memory/paging/paging.h"

/* Function to initialize */
 void init(u32int kernelPhysicalStart, u32int kernelPhysicalEnd) {
  /* Initialize segment descriptor tables */
  init_gdt();

  /* Configure serial port */
  serial_configure(SERIAL_COM1_BASE, Baud_115200);
  
  /* Initialize paging */
  init_paging(kernelPhysicalStart, kernelPhysicalEnd);
  
  /* Initialize idt */
  interrupts_install_idt();
  
}

/* Kernel Main */
 s32int kmain(u32int kernelPhysicalStart, u32int kernelPhysicalEnd) {
	
    	// Initialize all modules
   	init(kernelPhysicalStart, kernelPhysicalEnd);
  	 
  	return 0;
}
