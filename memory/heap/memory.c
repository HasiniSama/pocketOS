#include <stdint.h>
#include "string.h"
#include "memory.h"
#include "../../utils/log.h"

extern uint32_t kernel_end; // defined in link.ld
uint32_t next_free = (uint32_t)&kernel_end;

void *kmalloc(size_t size) {
  void *mem =  (void*)next_free;
  next_free += size;
  return mem;
}

void *kmalloc_page() {
  if (next_free & 0xfffff000) { // not aligned
    next_free &= 0xfffff000; // align it
    next_free += 0x1000; // advance to unallocated memory
  }
  debug("allocated page at %x", next_free);
  return kmalloc(0x1000);
}
