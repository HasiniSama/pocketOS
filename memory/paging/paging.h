#ifndef INCLUDE_PAGING_H
#define INCLUDE_PAGING_H

#pragma once
#include "../../utils/type.h"

typedef struct page {
  /* Page is present in memory if set */
  u32int present : 1;

  /* Read-only if clear, readwrite if set */
  u32int rw : 1;

  /* Supervisor level only if clear */
  u32int user : 1;

  /* Page been accessed since last refresh if set */
  u32int accessed : 1;

  /* Page been written to since last refresh if set */
  u32int dirty : 1;

  /* Amalgamation of unused and reserved bits */
  u32int unused : 7;

  /* Frame address (shifted right 12 bits). Since the frames are aligned at 4kb
   * least 12 bits are always zero.
   */
  u32int frame : 20;
} page_t;

typedef struct page_table {
  page_t pages[1024];
} page_table_t;

typedef struct page_directory {
  /* Array of pointers to pagetables. */
  page_table_t *tables[1024];

  /*
   * Array of pointers to the pagetables above, but gives their *physical*
   * location, for loading into the CR3 register.
   */
  u32int tablesPhysical[1024];

  /*
   * The physical address of tablesPhysical. This comes into play when we get
   * our kernel heap allocated and the directory may be in a different location
   * in virtual memory.
   */
  u32int physicalAddr;
} page_directory_t;

/** init_paging:
 *  Sets up the environment, page directories etc and enables paging.
 *
 *  @param kernelPhysicalEnd Physical address of location where loaded kernel
 *  ends
 */
void init_paging(u32int kernelPhysicalEnd);

/** switch_page_directory:
 *  Causes the specified page directory to be loaded into the CR3 register.
 *
 *  @param new Address of the new page directory to be switched to.
 */
void switch_page_directory(page_directory_t *new);

/** get_page:
 *  Retrieves a pointer to the page required. If make == 1, if the page-table in
 *  which this page should reside isn't created, create it!
 *
 *  @param address  Physical address is for which the virtual page is required
 *  @param make     Create page table if not created already.
 *  @param dir      Pointer to the page directory
 */
page_t *get_page(u32int address, u8int make, page_directory_t *dir);

/** alloc_frame:
 *  Marks that the frame is allocated based on the index calculated by page
 *  address. Also sets the page attributes in page table
 *
 *  @param page         Pointer to page address
 *  @param isKernel     If set, marks page accesible only in kernel mode
 *  @param isWriteable  If not set, markes page as read only
 */
void alloc_frame(page_t *page, u32int isKernel, u32int isWriteable);

/** free_frame:
 *  Marks the current page frame as not present and removes it from the list of
 *  frames that exist
 *
 *  @param page         Pointer to page address
 */
void free_frame(page_t *page);

#endif /* INCLUDE_PAGING_H */
