#include "paging.h"
#include "../heap/kheap.h"
#include "../../drivers/interrupts/interrupts.h"

// The kernel's page directory
page_directory_t *g_kernelDirectory = 0;

// The current page directory;
page_directory_t *g_currentDirectory = 0;

// A bitset of frames - used or free.
u32int *frames;
u32int nframes;

// Defined in kheap.c
extern u32int g_KerNelPhysicalAddressStart;
extern u32int g_CurrentPhysicalAddressTop;

// Macros used in the bitset algorithms.
#define INDEX_FROM_BIT(a) (a / 32)
#define OFFSET_FROM_BIT(a) (a % 32)

// Static function to set a bit in the frames bitset
static void set_frame(u32int frameAddr) {
  u32int frame = frameAddr / 0x1000;
  u32int idx = INDEX_FROM_BIT(frame);
  u32int off = OFFSET_FROM_BIT(frame);
  frames[idx] |= (0x1 << off);
}

// Static function to clear a bit in the frames bitset
static void clear_frame(u32int frameAddr) {
  u32int frame = frameAddr / 0x1000;
  u32int idx = INDEX_FROM_BIT(frame);
  u32int off = OFFSET_FROM_BIT(frame);
  frames[idx] &= ~(0x1 << off);
}

// // Static function to test if a bit is set.
// static u32int test_frame(u32int frameAddr) {
//   u32int frame = frameAddr / 0x1000;
//   u32int idx = INDEX_FROM_BIT(frame);
//   u32int off = OFFSET_FROM_BIT(frame);
//   return (frames[idx] & (0x1 << off));
// }

// Static function to find the first free frame.
static s32int first_frame() {
  u32int i, j;
   for (i = 0; i < INDEX_FROM_BIT(nframes); ++i) {
    if (frames[i] != 0xFFFFFFFF) {
      // at least one bit is free here.
       for (j = 0; j < 32; ++j) {
        u32int toTest = 0x1 << j;
        if (!(frames[i] & toTest)) {
          return ((i * 32) + j);
        }
      }
    }
  }
  // nothing free
  return -1;
}

// Function to allocate a frame with identity map.
void alloc_frame_identity(page_t *page, u32int isKernel, u32int isWriteable) {
  if (page->frame != 0) {
    return;
  } else {
    u32int idx = first_frame();
    if (idx == (u32int)-1) {
      while (1) {
      }
    }
    set_frame(idx * 0x1000);
    page->present = 1;
    page->rw = (isWriteable) ? 1 : 0;
    page->user = (isKernel) ? 0 : 1;
    page->frame = idx;
  }
}

void alloc_frame_virtual(page_t *page, u32int physicalAddress, u32int isKernel,
                         u32int isWriteable) {
  if (page->frame != 0) {
    return;
  } else {
    u32int idx = first_frame();
    set_frame(idx * 0x1000);

    idx = ((physicalAddress / 0x1000) / 32) + ((physicalAddress / 0x1000) % 32);
    page->present = 1;
    page->rw = (isWriteable) ? 1 : 0;
    page->user = (isKernel) ? 0 : 1;
    page->frame = idx;
  }
}

// Function to deallocate a frame.
void free_frame(page_t *page) {
  u32int frame;
  if (!(frame = page->frame)) {
    return;
  } else {
    clear_frame(frame);
    page->frame = 0x0;
  }
}

 void init_paging(u32int kerNelPhysicalStart, u32int kernelPhysicalEnd) {

   set_physical_address(kerNelPhysicalStart, kernelPhysicalEnd);
  /* The size of physical memory.
   * Assuming it is 16MB big setting memEndPage to 16MB + 3GB so that we can
   * relocate kernel to 3GB
   */
  u32int memEndPage = 0xC1000000;

  nframes = memEndPage / 0x1000;
  frames = (u32int *)kmalloc(INDEX_FROM_BIT(nframes) + 1);
  custom_memset((u8int *)frames, 0, INDEX_FROM_BIT(nframes));

  // Let's make a page directory.
  g_kernelDirectory = (page_directory_t *)kmalloc_a(sizeof(page_directory_t));
  g_currentDirectory = g_kernelDirectory;

  /* We need to identity map (phys addr = virt addr) from 0x0 to the end of
   * used memory, so we can access this transparently, as if paging wasn't
   * enabled.
   */
  u32int i = 0;
  
  while (i < (g_KerNelPhysicalAddressStart & 0xFFFFF000)) {
    /* identity map 1 MB which is BIOS */
    alloc_frame_identity(get_page(i, 1, g_kernelDirectory), 0, 0);
    i += 0x1000;
  }
  
  while (i < g_CurrentPhysicalAddressTop) {
    // Kernel code is readable but not writeable from userspace.
    alloc_frame(get_page(i, 1, g_kernelDirectory), 0, 0);
    /* Since we have already relocated kernel to 1MB after 3GB in linker script,
     * we can add entry to page table by adding 3Gb to all the physical
     * address.
     * Kernel code is readable but not writeable from userspace.
     */
    alloc_frame_virtual(get_page(i + 0xC0100000, 1, g_kernelDirectory), i, 0,
                        0);
    i += 0x1000;
  }
  // Before we enable paging, we must register our page fault handler.
  //register_interrupt_handler(14, page_fault);

  // Now, enable paging!
  switch_page_directory(g_kernelDirectory);
}

void switch_page_directory(page_directory_t *dir) {
  g_currentDirectory = dir;
  // Write page table physical address to cr3
  asm volatile("mov %0, %%cr3" ::"r"(&dir->tablesPhysical));
  u32int cr0;
  // Read cr0 register to variable cr0
  asm volatile("mov %%cr0, %0" : "=r"(cr0));
  // Enable paging!
  cr0 |= 0x80000000;
  // Write back to cr0
  asm volatile("mov %0, %%cr0" ::"r"(cr0));
}

page_t *get_page(u32int address, u8int make, page_directory_t *dir) {
  // Turn the address into an index.
  address /= 0x1000;
  // Find the page table containing this address.
  u32int tableIdx = address / 1024;
  // If this table is already assigned
  if (dir->tables[tableIdx]) {
    return &dir->tables[tableIdx]->pages[address % 1024];
  } else if (make) {
    u32int tmp;
    dir->tables[tableIdx] =
        (page_table_t *)kmalloc_ap(sizeof(page_table_t), &tmp);
        custom_memset((u8int *)dir->tables[tableIdx], 0, 0x1000);
    // PRESENT, RW, US.
    dir->tablesPhysical[tableIdx] = tmp | 0x7;
    return &dir->tables[tableIdx]->pages[address % 1024];
  } else {
    return 0;
  }
}
