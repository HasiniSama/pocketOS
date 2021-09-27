#include "fb.h"	

static s8int *fb = (s8int *)0x000B8000;

void fb_move_cursor(u16int pos) {
	outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
	outb(FB_DATA_PORT, ((pos >> 8) & 0x00FF));
	outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
  	outb(FB_DATA_PORT, pos & 0x00FF);
}

void fb_write_cell(u32int i, s8int c, u8int fg, u8int bg) {
	fb[i] = c;
	fb[i + 1] = ((fg & 0x0F) << 4) | (bg & 0x0F);
}

void fb_write(s8int c, u32int i){
	    	fb_write_cell(i*2, c, LIGHT_GREY, BLUE);
	    	fb_move_cursor(2);
}

void fb_clear(u32int i){

  fb_write_cell(i*2, ' ', BLACK, BLACK);
  
}
