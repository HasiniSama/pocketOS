    	global loader                   ; the entry symbol for ELF
    
    	KERNEL_STACK_SIZE equ 4096      ; size of stack in bytes
    
     	;setting up the Multiboot header - see GRUB docs for details			
    	; align loaded modules on page boundaries
	MODULEALIGN equ 1 << 0

	; provide memory map
	MEMINFO equ 1 << 1

	; this is the Multiboot 'flag' field
	FLAGS equ MODULEALIGN | MEMINFO

	; 'magic number' lets bootloader find the header
	MAGIC_NUMBER equ 0x1BADB002

	; calculate the checksum (all options + checksum should equal 0)
    	CHECKSUM equ - (MAGIC_NUMBER + FLAGS)

	; Kernel End Physical Address is exported from linker script
	extern KERNEL_PHYSICAL_END

	; start of the bss section
    section .bss
    align 4                         ; align at 4 bytes
    KERNEL_STACK:                   ; label points to beginning of memory
        resb KERNEL_STACK_SIZE      ; reserve stack for the kernel
    
        
    section .text                   ; start of the text (code) section
    align 4                         ; the code must be 4 byte aligned
        dd MAGIC_NUMBER             ; write the magic number to the machine code,
        dd FLAGS            ; write the align modules instruction
        dd CHECKSUM                 ; write the checksum
    
    	; the loader label (defined as entry point in linker script)
    loader:
    	mov esp, KERNEL_STACK + KERNEL_STACK_SIZE  ; point esp to the start of the stack (end of memory area)
    	
    	extern kmain			; kmain function is defined elsewhere
    	
    	push $KERNEL_PHYSICAL_END	; and the kernel end address
    	
    	call kmain			; call kernel main function.
    .loop:
    	; loop forever
      jmp .loop
