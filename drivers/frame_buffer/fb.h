#ifndef INCLUDE_FRAMEBUFFER_H
#define INCLUDE_FRAMEBUFFER_H

#pragma once
#include "../io/io.h"
#include "../../utils/type.h"

/* The I/O ports */
#define FB_COMMAND_PORT 0x3D4
#define FB_DATA_PORT 0x3D5

/* The I/O port commands */
#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND 15

/** fb_move_cursor:
 *  Moves the cursor of the framebuffer to the given position
 *
 *  @param pos The new position of the cursor
 */
void fb_move_cursor(u16int pos);

/** fb_write_cell:
 *  Writes a character with the given foreground and background to position i
 *  in the framebuffer.
 *
 *  @param i  The location in the framebuffer
 *  @param c  The character
 *  @param fg The foreground color
 *  @param bg The background color
 */
void fb_write_cell(u32int i, s8int c, u8int fg, u8int bg);

/** write:
 *  writes the contents of the buffer buf of length len to the screen
 *
 *  @param c  character that needs to be written to screen
 *  @param i  number of total characters written on the framebuffer
 */
void fb_write(s8int c, u32int i);

/** clear:
 *  clears the previous content of the buffer and move the cursor back
 *
 *  @param i    number of total characters written on the framebuffer
 */
void fb_clear(u32int i);

#endif /* INCLUDE_IO_H */
