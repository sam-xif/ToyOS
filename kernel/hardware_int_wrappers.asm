;
; hardware_int_wrappers.asm
;

global _timer_interrupt_wrapper
global _kybrd_interrupt_wrapper
global _dbz_wrapper


extern _timer_interrupt
extern _divide_by_zero_fault
extern _kybrd_interrupt

section .text

; Hardware interrupt 0: Divide by zero
_dbz_wrapper:
  pusha
  
  call _divide_by_zero_fault ; call divide by zero interrupt handler
  
  mov al, 0x20
  out 0x20, al
  
  popa
  iret

; Hardware interrupt 32: Timer
_timer_interrupt_wrapper:
  pusha
  
  call _timer_interrupt
  
  mov al, 0x20 ; EOI
  out 0x20, al
  
  popa
  iret
  
_kybrd_interrupt_wrapper:
  pusha
  
  call _kybrd_interrupt
  
  mov al, 0x20 ; EOI
  out 0x20, al
  
  popa
  iret