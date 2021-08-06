#include "frame_buffer.c"

void kmain(){
    
	char buffer[] = "Hello Reader! How are you today?";
	fb_write(buffer,sizeof(buffer));
       
}
