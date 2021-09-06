#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stddef.h>

void *kmalloc(size_t size);
void *kmalloc_page();

#endif
