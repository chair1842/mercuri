.section .multiboot
.align 4
.long 0x1BADB002             # magic number
.long 0x0                    # flags
.long -(0x1BADB002 + 0x0)    # checksum

.section .text
.global _start
.type _start, @function

_start:
    cli                     # Disable interrupts
    call kernel_main        # Call the C kernel function
.hang:
    hlt                     # Halt CPU
    jmp .hang               # Infinite loop
