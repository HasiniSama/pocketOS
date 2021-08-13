#include "drivers/frame_buffer.c"
#include "drivers/serial_port.c"
#include "segmentation/memory_segments.h"

/* Function to initialize */
void init() {
  /* Initialize segment descriptor tables */
  init_gdt();

  /* Initialize serial port */
  serial_configure(SERIAL_COM1_BASE, Baud_115200);
}

/* Kernel Main */
void kmain(){
    	init();
	char buffer[] = "Welcome to pocketOS!";
	
	fb_write(buffer,sizeof(buffer));
	serial_write(SERIAL_COM1_BASE, buffer, sizeof(buffer));
  	
}
