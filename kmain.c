#include "drivers/serial_port.c"
#include "segmentation/memory_segments.h"
#include "interrupt/interrupts.h"
#include "multiboot.h"

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
int kmain(unsigned int ebx){
    	init();
    	
  	multiboot_info_t *mbinfo = (multiboot_info_t *) ebx;
  	multiboot_module_t* modules = (multiboot_module_t*) mbinfo->mods_addr; 
  	unsigned int address_of_module = modules->mod_start;
  	 
  	return 0;
}
