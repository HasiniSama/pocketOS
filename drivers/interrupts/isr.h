#ifndef __ISR_H__
#define __ISR_H__

#include <stdint.h>

#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

typedef struct registers {
  uint32_t ds; // data segment
  uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // pushed by pusha
  uint32_t int_no, err_code; // interrupt number & error code
  uint32_t eip, cs, eflags, usereap, ss; // pushed by processor
} registers_t; // TODO, shouldn't this be packed?

// Enables registration of callbacks for interrupts or IRQs.
// For IRQs, to ease confusion, use the #defines above as the
// first parameter.
typedef void (*isr_t)(registers_t);

void ack_irq(int int_no);

void register_interrupt_handler(uint8_t n, isr_t handler);

#endif
