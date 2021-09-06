#ifndef __PAGING_H__
#define __PAGING_H__

#include <stdint.h>

struct directory {
  uint8_t present:1;        // bit 0: always 1
  uint8_t rw:1;             // bit 1: read/write
  uint8_t us:1;             // bit 2: user or supervisor
  uint8_t pwt:1;            // bit 3: page-level write-through
  uint8_t pcd:1;            // bit 4: page-level cache disable
  uint8_t a:1;              // bit 5: accessed
  uint8_t ignored:1;        // bit 6:
  uint8_t ps:1;             // bit 7: page size, 0=4KB 1=4MB, must be 0 for this struct
  uint8_t ignored2:4;       // bit 8 - 11
  uint32_t page_table:20;   // bit 12 - 31: physical address of 4KB aligned page table referenced by this entry
} __attribute__((packed));
typedef struct directory directory_t;

struct page {
  uint8_t present:1;        // bit 0: always 1
  uint8_t rw:1;             // bit 1: read/write
  uint8_t us:1;             // bit 2: user or supervisor
  uint8_t pwt:1;            // bit 3: page-level write-through
  uint8_t pcd:1;            // bit 4: page-level cache disable
  uint8_t a:1;              // bit 5: accessed
  uint8_t d:1;              // bit 6: dirty
  uint8_t pat:1;            // bit 7: must be 0 unless PAT supported
  uint8_t g:1;              // bit 8: global translation
  uint8_t ignored2:3;       // bit 9 - 11
  uint32_t page_frame:20;   // bit 12 - 31: physical address of 4KB page frame
} __attribute__((packed));
typedef struct page page_t;

void init_paging();

#endif
