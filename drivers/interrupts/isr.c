#include "isr.h"
#include "string.h"
#include "../io/io.h"


#define PIC1            0x20    /* IO base address for master PIC */
#define PIC2            0xA0    /* IO base address for slave PIC */
#define PIC1_COMMAND    PIC1
#define PIC1_DATA       (PIC1+1)
#define PIC2_COMMAND    PIC2
#define PIC2_DATA       (PIC2+1)

#define PIC_EOI         0x20    /* End-of-interrupt command code */

isr_t interrupt_handlers[256];

void isr_handler(registers_t regs) {
  if (interrupt_handlers[regs.int_no] != 0) {
    isr_t handler = interrupt_handlers[regs.int_no];
    handler(regs);
  } else {
    //debug("unhandled s/w interrupt: %i\n", regs.int_no);
    //debug("eip: %x\n", regs.eip);
  }
}

void ack_irq(int int_no) {
  // Send an EOI (end of interrupt) signal to the PICs.
  // If this interrupt involved the slave.
  if (int_no >= 40)
  {
     // Send reset signal to slave.
     outb(PIC2_COMMAND, PIC_EOI);
  }
  // Send reset signal to master. (As well as slave, if necessary).
  outb(PIC1_COMMAND, PIC_EOI);
}

// This gets called from our ASM interrupt handler stub.
void irq_handler(registers_t regs)
{
  //debug("h/w interrupt: %i\n", regs.int_no);

  ack_irq(regs.int_no);

  if (interrupt_handlers[regs.int_no] != 0) {
     isr_t handler = interrupt_handlers[regs.int_no];
     handler(regs);
  }
}

void register_interrupt_handler(uint8_t n, isr_t handler)
{
  interrupt_handlers[n] = handler;
}
