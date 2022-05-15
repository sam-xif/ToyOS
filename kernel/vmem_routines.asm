global _set_page_table_root
global _get_page_table_root

section .text


_set_page_table_root:
  ; first arg is at [ebp + 8]
  push ebp
  mov ebp, esp
  push eax
  mov eax, [ebp + 8]
  mov cr3, eax
  pop eax
  pop ebp
  ret

_get_page_table_root:
  mov eax, cr3
  ret  
