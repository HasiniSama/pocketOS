#ifndef INCLUDE_MEMORY_SEGMENTS
#define INCLUDE_MEMORY_SEGMENTS

#include "../../utils/type.h"

/* Access ASM function from C code. */
extern void gdt_flush(u32int);

/* This struct describes a GDT pointer. It points to the start of
 * our array of GDT entries, and is in the format required by the
 * lgdt instruction.
 */
struct gdt_ptr_struct 
{
	u16int size;	// The upper 16 bits of all selector limits.
	u32int address;	// The address of the first gdt_entry_t struct.
} __attribute__((packed));

typedef struct gdt_ptr_struct gdt_ptr_t;


/* This structure contains the value of one GDT entry.
 * We use the attribute 'packed' to tell GCC not to change
 * any of the alignment in the structure.
 */
struct gdt_entry_struct
{
	u16int limit_low;	// The lower 16 bits of the limit.
	u16int base_low;	// The lower 16 bits of the base.
	u8int base_middle;	// The next 8 bits of the base.
	u8int access_byte;	// Access flags, determine what ring this segment can be used in.
	u8int granularity;	
	u8int base_high;	// The last 8 bits of the base.
} __attribute__((packed));

typedef struct gdt_entry_struct gdt_entry_t;

/* Set the value of GDT entry. */
void gdt_set_gate(s32int index, u32int base_address, u32int limit, u8int access_byte, u8int gran);

/* Function to initialize GDT */
void init_gdt();


#endif /* INCLUDE_MEMORY_SEGMENTS */
