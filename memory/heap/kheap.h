#ifndef INCLUDE_KHEAP_H
#define INCLUDE_KHEAP_H

#pragma once
#include "../../utils/type.h"

/* set_physical_address_top:
 * set the top of physical address global variable. This is required to keep
 *  track of the memory thats allocated till now. Initially, this is set to top
 *  of address where kernel ends. We keep moving this as memory is allocated.
 *
  *  @param phyAddress Current Address End, we get this from linker script. script.
 */
void set_physical_address_top(u32int phyAddress);

/* kmalloc_int:
 *  Allocate a chunk of memory, size in size. If align == 1,
 *  the chunk must be page-aligned. If pAddrPtr != 0, the physical
 *  location of the allocated chunk will be stored into phys.
 *
 *  @param size      Size of memory to be allocated
 *  @param align     Align allocated memory at 4kb address
 *  @param pAddrPtr  Store the physical address of the page
 */
 u32int kmalloc_int(u32int size, u32int align, u32int *pAddrPtr);

/* kmalloc_a:
 *  Allocate a chunk of memory, size in size. The chunk must be
 *  page aligned.
 *
 *  @param size Size of memory to be allocated
 */
u32int kmalloc_a(u32int size);

/* kmalloc_p:
 *  Allocate a chunk of memory, size in size. The physical address
 *  is returned in pAddrPtr. pAddrPtr MUST be a valid pointer to u32int!
 *
 *  @param pAddrPtr  Pointer to store the physical address of
 *  the page
 */
u32int kmalloc_p(u32int sz, u32int *pAddrPtr);

/* kmalloc_p:
 *  Allocate a chunk of memory, size in size. The physical address
 *  is returned in pAddrPtr. It must be page-aligned.
 *
 *  @param size      Size of memory to be allocated
 *  @param pAddrPtr  Pointer to store the physical address of
 *  the page
 */
u32int kmalloc_ap(u32int size, u32int *pAddrPtr);

/* kmalloc:
 *  Generic function to allocate chunk of memory, size in size.
 *
 *  @param size  Size of memory to be allocated
 */
u32int kmalloc(u32int size);

#endif /* INCLUDE_KHEAP_H */
