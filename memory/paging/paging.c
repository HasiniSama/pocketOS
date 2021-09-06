#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "paging.h"
#include "string.h"
#include "../heap/memory.h"
#include "../../utils/log.h"
#include "../../drivers/interrupts/isr.h"
#include "../../drivers/serial_port/serial.h"

extern directory_t boot_page_directory; // pre-allocated by nasm (also page aligned)

uint32_t frame_allocations[32767];

#define PAGE_FRAME_SIZE 4096

// addr must be page aligned
static bool test_frame(uint32_t addr) {
  uint32_t frame = addr / PAGE_FRAME_SIZE;
  uint32_t frame_word = frame / 32;
  uint32_t bit_offset = frame % 32;
  return  (frame_allocations[frame_word] >> bit_offset) & 1;
}

// addr must be page aligned
static void set_frame(uint32_t addr) {
  uint32_t frame = addr / PAGE_FRAME_SIZE;
  uint32_t frame_word = frame / 32;
  uint32_t bit_offset = frame % 32;
  frame_allocations[frame_word] |= (1 << bit_offset);
}

// addr must be page aligned
static void clear_frame(uint32_t addr) {
  uint32_t frame = addr / PAGE_FRAME_SIZE;
  uint32_t frame_word = frame / 32;
  uint32_t bit_offset = frame % 32;
  frame_allocations[frame_word] &= ~(1 << bit_offset);
}

void handle_page_fault(registers_t regs) {
  debug("Page fault!");
}

#define PAGE_READONLY   0
#define PAGE_READWRITE  1
#define PAGE_USER       1
#define PAGE_KERNEL     0
#define PAGE_SIZE_4KB   0
#define PAGE_SIZE_4MB   1

void map(directory_t *page_directory, uint32_t vaddr, uint32_t paddr) {
  debug("\nidentity mapping %x-%x\n", addr & 0xfffff000, (addr & 0xfffff000) + 0x1000 - 1);

  uint32_t directory_offset = vaddr >> 22; // 31:22
  uint32_t table_offset = (vaddr >> 12) & 0x3ff; // 21:12
  debug("\tdirectory offset: %x\n", directory_offset);
  debug("\ttable offset: %x\n", table_offset);

  directory_t *directory = &page_directory[directory_offset/4];
  page_t *table;
  // debug("kernel page directory resides at %x\n", page_directory);
  if (!directory->present) {
    // debug("configuring new page directory entry with index %i at %x\n", directory_offset, directory);
    directory->present = 1;
    directory->rw = PAGE_READWRITE;
    directory->us = PAGE_KERNEL;
    directory->ps = PAGE_SIZE_4KB;

    table = (page_t*)kmalloc_page(); // sizeof(page_t)*1024 == 0x1000
    memset(table, 0, 0x1000);

    directory->page_table = (uint32_t)table >> 12; // table base (20 high bits)
    debug("directory->page_table is now %x\n", directory->page_table << 12);
  } else {
    table = (page_t*)(directory->page_table << 12);
    debug("c directory->page_table already exists at %x\n", table);
  }

  page_t *page = &table[table_offset];
  if (!page->present) {
    debug("configuring new page table entry at %x\n", page);
    page->present = 1;
    page->rw = PAGE_READWRITE;
    page->us = PAGE_KERNEL;
  }

  page->page_frame = paddr >> 12; // frame base (20 high bits)
  debug("page %x is now mapped to frame %x\n", addr, page->page_frame << 12);
  set_frame(paddr);
}

extern void load_page_directory(directory_t *directory);
extern void enable_paging();

void init_paging() {
  info("Initializing paging");

  // listen for page faults
  register_interrupt_handler(14, handle_page_fault);

  debug("boot_page_directory=%x", &boot_page_directory);

  load_page_directory(&boot_page_directory);
  enable_paging();
  info("Paging enabled");

  char message[] = "Paging enabled successfully!!\n";
  serial_write(0x3F8,message,sizeof(message));
}
