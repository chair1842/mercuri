.section .data
.align 8
.global gdt_descriptor

gdt:
    .quad 0x0000000000000000    # Null descriptor

    # Kernel code segment: base=0x0, limit=0xFFFFF, flags=0x9A (execute/read, ring 0)
    .word 0xFFFF                # Limit (low)
    .word 0x0000                # Base (low)
    .byte 0x00                  # Base (middle)
    .byte 0x9A                  # Access byte
    .byte 0xCF                  # Flags (limit high 4 bits + granularity)
    .byte 0x00                  # Base (high)

    # Kernel data segment: base=0x0, limit=0xFFFFF, flags=0x92 (read/write, ring 0)
    .word 0xFFFF
    .word 0x0000
    .byte 0x00
    .byte 0x92
    .byte 0xCF
    .byte 0x00

gdt_descriptor:
    .word gdt_end - gdt - 1     # Limit
    .long gdt                   # Base address

gdt_end:
