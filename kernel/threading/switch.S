; switch.S
bits 32
global switch_thread
switch_thread:
    ; Args: [esp + 4] = old_sp ptr, [esp + 8] = new_sp
    mov  eax, [esp + 4]    ; address of old_sp in C
    mov  edx, [esp + 8]    ; new_sp value from C

    mov  [eax], esp        ; *old_sp = current ESP
    mov  esp, edx          ; ESP = new_sp

    ; Return to next thread’s context (pop pusha, iret frame)
    ret
