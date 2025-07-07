.global gdt_load
.type gdt_load, @function
.extern gdt_descriptor 
gdt_load:
    lgdt gdt_descriptor         # Load GDT

    mov $0x10, %ax              # 0x10 = offset of data segment (3rd entry, index 2)
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    ljmp $0x08, $reload_cs      # Far jump to reload CS (0x08 = code segment offset)

reload_cs:
    ret
