#include "memory_segments.h"

#define SEGMENT_DESCRIPTOR_COUNT 3

#define SEGMENT_BASE 0
#define SEGMENT_LIMIT 0xFFFFF

#define SEGMENT_CODE_TYPE 0x9A
#define SEGMENT_DATA_TYPE 0x92

#define SEGMENT_GRANULARITY 0xCF

static gdt_entry_t gdt_entries[SEGMENT_DESCRIPTOR_COUNT];

void gdt_set_gate(int index, unsigned int base_address, unsigned int limit, unsigned char access_byte, unsigned char gran)
{
	gdt_entries[index].base_low = base_address & 0xFFFF;
	gdt_entries[index].base_middle = (base_address >> 16) & 0xFF;
	gdt_entries[index].base_high = (base_address >> 24) & 0xFF;

	gdt_entries[index].limit_low = limit & 0xFFFF;
	
	/*
   * name | value | size | desc
   * ---------------------------
   * P    |     1 |    1 | segment present in memory
   * DPL  |    pl |    2 | privilege level
   * S    |     1 |    1 | descriptor type, 0 = system, 1 = code or data
   * Type |  type |    4 | segment type, how the segment can be accessed
   * Access = 0x9A privilege 0 code segment, Access = 0x92 for privilege 0 data
   * segment
   */
  	gdt_entries[index].access_byte = access_byte;
  	
  	/*
   * name | value | size | desc
   * ---------------------------
   * G    |     1 |    1 | granularity, size of segment unit, 1 = 4kB
   * D/B  |     1 |    1 | size of operation size, 0 = 16 bits, 1 = 32 bits
   * L    |     0 |    1 | 1 = 64 bit code
   * AVL  |     0 |    1 | "available for use by system software"
   * LIM  |   0xF |    4 | the four highest bits of segment limit
   * Granularity = 0xCF as far as highest bits of segment limit is 0xFFFFFFFF
   */
  	gdt_entries[index].granularity = gran;

}

void init_gdt()
{
	gdt_ptr_t gdt_ptr;
	gdt_ptr.size = (sizeof(gdt_entry_t) * 3) - 1;
	gdt_ptr.address = (unsigned int)&gdt_entries;

  	gdt_set_gate(0, 0, 0, 0, 0);                // Null segment
  	
  	gdt_set_gate(1, SEGMENT_BASE, SEGMENT_LIMIT, SEGMENT_CODE_TYPE, SEGMENT_GRANULARITY); // Kernel Code segment
  	gdt_set_gate(2, SEGMENT_BASE, SEGMENT_LIMIT, SEGMENT_DATA_TYPE, SEGMENT_GRANULARITY); // Kernel Data segment

  	gdt_flush((unsigned int)&gdt_ptr);
	
}
