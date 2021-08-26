	; set eax to some distinguishable number, to read from the log afterwards
	bits 32
	start:
	mov eax, 0xDEADBEEF
	; enter infinite loop, nothing more to do
	jmp start
