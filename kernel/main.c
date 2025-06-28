#include <vga.h>
#include <driver/keyboard.h>
#include <memory/memory.h>
#include <memory/paging.h>
#include <interrupts/idt.h>
#include <interrupts/pic.h>
#include "shell/shell.c"

void kernel_init() {
    clear_screen();
    memory_init();
    print_string("Memory Initialized\n");
    delay(100000000);
    paging_init();
    print_string("Paging Initialized\n");
    delay(100000000);

    pic_remap(0x20, 0x28);
    print_string("PIC Remapped\n");
    delay(100000000);
    idt_init();
    print_string("IDT Initialized\n");
    delay(100000000);

    keyboard_init();
    print_string("Keyboard Initialized\n");
    delay(100000000);

    print_string("Kernel Initialized\n");
    delay(100000000);
    clear_screen();
}


void kernel_main() {
    kernel_init();
    shell_init();
}
    


