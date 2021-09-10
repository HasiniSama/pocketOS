#include "drivers/serial_port/serial.c"
#include "memory/segmentation/memory_segments.h"
#include "drivers/interrupts/interrupts.h"
#include "multiboot.h"
#include "utils/type.h"

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
int kmain(u32int ebx){
    	init();
    	
  	multiboot_info_t *mbinfo = (multiboot_info_t *) ebx;
  	multiboot_module_t* modules = (multiboot_module_t*) mbinfo->mods_addr; 
  	u32int address_of_module = modules->mod_start;
  	
  	if((mbinfo->mods_count) == 1){
  		s8int message[] = "ONE module loaded successfully!";
  		serial_write(0x3F8,message,sizeof(message));
  		
  		typedef void (*call_module_t)(void);
        	/* ... */
        	call_module_t start_program = (call_module_t) address_of_module;
        	start_program();
        	/* we'll never get here, unless the module code returns */

  	}else{
  		s8int message[] = "Error: More than ONE module loaded";
  		serial_write(0x3F8,message,sizeof(message));
  	}
  	 
  	return 0;
}
