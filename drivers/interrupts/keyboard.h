#ifndef INCLUDE_KEYBOARD_H
#define INCLUDE_KEYBOARD_H

#define KEYBOARD_MAX_ASCII 83 

#include "../../utils/type.h"

u8int keyboard_read_scan_code(void);

u8int keyboard_scan_code_to_ascii(u8int);

#endif /* INCLUDE_KEYBOARD_H */

