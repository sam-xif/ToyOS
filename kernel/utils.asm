; Utility functions

global _readport
global _writeport

section .text

; byte readport()
_readport:
  push ebp
  mov ebp, esp
  
  mov eax, 0
  mov ecx, [ebp + 8]
  in al, cl
  
  pop ebp
  ret
  
; void writeport(byte port, byte cmd)  
_writeport:
  push ebp
  mov ebp, esp
  
  mov byte al, [ebp + 8] ; port
  mov byte ah, [ebp + 12] ; cmd
  
  out al, ah
  
  pop ebp
  ret