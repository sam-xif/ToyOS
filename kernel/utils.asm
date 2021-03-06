; Utility functions

global _readport
global _writeport

section .text

; byte readport(byte port)
_readport:
  push ebp
  mov ebp, esp
  
  xor eax, eax
  mov edx, eax
  
  mov edx, [ebp + 8]
  in al, dx
  
  pop ebp
  ret
  
; void writeport(byte port, byte cmd)  
_writeport:
  push ebp
  mov ebp, esp
  
  ; Zero out the registers that we are going to use
  xor edx, edx
  mov eax, edx
  
  mov edx, [ebp + 8] ; port
  mov byte al, [ebp + 12] ; cmd
  
  out dx, al
  
  pop ebp
  ret