extern USER_DATA_SEGMENT_SELECTOR
extern USER_CODE_SEGMENT_SELECTOR

global switch_to_user_mode

switch_to_user_mode:
  mov ax,0x23 ;user data segment with bottom 2 bits set for ring 3
  mov ds,ax
  mov es,ax
  mov fs,ax
  mov gs,ax ;we don't need to worry about SS. it's handled by iret

  mov eax,esp
  push 0x23 ;user data segment with bottom 2 bits set for ring 3
  push eax ; the user mode stack pointer
  pushf ; push current eflags
  push 0x1B ; user code segment with bottom 2 bits set for ring 3
  push 0x1f ; the instruction pointer of user mode code to execute
  iret ; this copies values from the stack into the appropriate registers
