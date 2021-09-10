#ifndef INCLUDE_INTERRUPTS
#define INCLUDE_INTERRUPTS

#include "../../utils/type.h"

struct IDT 
{
	u16int size;
	u32int address;
} __attribute__((packed));

struct IDTDescriptor {
	/* The lowest 32 bits */
	u16int offset_low; // offset bits 0..15
	u16int segment_selector; // a code segment selector in GDT or LDT
	
	/* The highest 32 bits */
	u8int reserved; // Just 0.
	u8int type_and_attr; // type and attributes
	u16int offset_high; // offset bits 16..31
} __attribute__((packed));

void interrupts_install_idt();

// Wrappers around ASM.
void load_idt(u32int idt_address);
void interrupt_handler_33();
void interrupt_handler_14();

struct cpu_state {
	u32int eax;
	u32int ebx;
	u32int ecx;
	u32int edx;
	u32int ebp; 
	u32int esi; 
	u32int edi; 
} __attribute__((packed));

struct stack_state {
	u32int error_code;
	u32int eip;
	u32int cs;
	u32int eflags;
} __attribute__((packed));

void interrupt_handler(struct cpu_state cpu, u32int interrupt, struct stack_state stack);


#endif /* INCLUDE_INTERRUPTS */
