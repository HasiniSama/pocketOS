#include "drivers/serial_port/serial.c"
#include "memory/segmentation/memory_segments.h"
#include "drivers/interrupts/interrupts.h"
#include "multiboot.h"
#include "utils/type.h"
#include "memory/paging/paging.h"
#include "memory/heap/kheap.h"
#include "user_mode.h"
#include "drivers/interrupts/hardware_interrupt_enabler.h"

/* Function to initialize */
void init(u32int kernelPhysicalEnd) {
  /* Initialize segment descriptor tables */
  init_gdt();

  /* Configure serial port */
  serial_configure(SERIAL_COM1_BASE, Baud_115200);
  
  /* Initialize paging */
  init_paging(kernelPhysicalEnd);
  
  /* Initialize idt */
  interrupts_install_idt();
  
}

/* Kernel Main */
int kmain(u32int kernelPhysicalEnd){
	
    	// Initialize all modules
  	init(kernelPhysicalEnd);
  	
  	disable_hardware_interrupts();
  	
  	// Switch to User mode
   	switch_to_user_mode();
   	
	 
  	return 0;
}
