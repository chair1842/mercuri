#include <vga.h>
#include <driver/keyboard.h>
#include <memory/memory.h>
#include <memory/paging.h>
#include <interrupt.h>


void delay(volatile int count) {
    for (volatile int i = 0; i < count; i++);
}

void kernel_init() {
    clear_screen();
    memory_init();
    print_string("Memory Initialized\n");
    paging_init();
    print_string("Paging Initialized\n");
    interrupt_init();
    print_string("Interrupts Initialized\n");

    print_string("Kernel Initialized\n");
    clear_screen();
}


void kernel_main() {
    kernel_init();

    // Type any key to continue
    keyboard_init();
    print_string("Type any key to continue...\n");

    // Clear leftover keystrokes
    while (keyboard_available()) {
        keyboard_read_char();
    }

    keyboard_get_char();
    clear_screen();
    print_string("Hello, World! :)\n");
}
    


