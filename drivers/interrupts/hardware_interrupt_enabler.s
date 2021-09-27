global enable_hardware_interrupts

enable_hardware_interrupts:
  sti
  ret

global disable_hardware_interrupts

disable_hardware_interrupts:
  cli
  ret
