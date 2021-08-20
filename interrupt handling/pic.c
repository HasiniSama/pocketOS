#include "../drivers/io.h"
#include "pic.h"

/*
        Programmable Interrupt Controller
	From: http://wiki.osdev.org/PIC
	Reinitialize the PIC controllers, giving them specified vector offsets
	rather than 8h and 70h, as configured by default.
*/

/**
  *  Acknowledges an interrupt from either PIC 1 or PIC 2.
  *
  *  @param num The number of the interrupt
  */
void pic_acknowledge(unsigned int interrupt)
{
	if (interrupt < PIC_1_OFFSET || interrupt > PIC_2_END) {
		return;
	}

	if (interrupt < PIC_2_OFFSET) {
		outb(PIC_1_COMMAND_PORT, PIC_ACKNOWLEDGE);
	} else {
		outb(PIC_2_COMMAND_PORT, PIC_ACKNOWLEDGE);
	}
}

/*
arguments:
	offset1 - vector offset for master PIC
		vectors on the master become offset1..offset1+7
	offset2 - same for slave PIC: offset2..offset2+7
*/
void pic_remap(int offset1, int offset2)
{
	outb(PIC_1_COMMAND, PIC_ICW1_INIT + PIC_ICW1_ICW4);	// starts the initialization sequence (in cascade mode)
	outb(PIC_2_COMMAND, PIC_ICW1_INIT + PIC_ICW1_ICW4);
	outb(PIC_1_DATA, offset1);				// ICW2: Master PIC vector offset
	outb(PIC_2_DATA, offset2);				// ICW2: Slave PIC vector offset
	outb(PIC_1_DATA, 4);					// ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	outb(PIC_2_DATA, 2);					// ICW3: tell Slave PIC its cascade identity (0000 0010)

	outb(PIC_1_DATA, PIC_ICW4_8086);
	outb(PIC_2_DATA, PIC_ICW4_8086);

        // Setup Interrupt Mask Register (IMR)
	outb(PIC_1_DATA, 0xFD); // 1111 1101 - Enable IRQ 1 only (keyboard).
	outb(PIC_2_DATA, 0xFF);

	asm("sti"); // Enable interrupts.
}
