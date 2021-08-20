#include "drivers/serial_port.c"
#include "segmentation/memory_segments.h"
#include "interrupt/interrupts.h"

/* Function to initialize */
void init() {
  /* Initialize segment descriptor tables */
  init_gdt();

  /* Initialize serial port */
  serial_configure(SERIAL_COM1_BASE, Baud_115200);
  
  /* Initialize idt */
  interrupts_install_idt();
}

/* Kernel Main */
void kmain(){
    	init();
  	
}
