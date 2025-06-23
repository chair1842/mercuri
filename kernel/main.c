#include <vga.h>
#include <driver/keyboard.h>
#include <memory/memory.h>
#include <memory/paging.h>
#include <interrupts/idt.h>

void delay(volatile int count) {
    for (volatile int i = 0; i < count; i++);
}

void kernel_init() {
    clear_screen();
    memory_init();
    print_string("Memory Initialized\n");
    delay(100000000);
    paging_init();
    print_string("Paging Initialized\n");
    delay(100000000);
    idt_init();
    print_string("IDT Initialized\n");
    delay(100000000);


    print_string("Kernel Initialized\n");
    delay(100000000);
    clear_screen();
}


void kernel_main() {
    /* kernel_init();

    // Type any key to continue
    keyboard_init();
    print_string("Type any key to continue...\n");

    keyboard_get_char();
    clear_screen();
    print_string("Hello, World! :)\n"); */

    clear_screen();
    idt_init();

    int a = 1 /0;
}
    


