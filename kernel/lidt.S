.intel_syntax noprefix
.global lidt
.section .text

lidt:
    mov eax, [esp + 4]       // get the pointer argument
    lidt [eax]
    ret
