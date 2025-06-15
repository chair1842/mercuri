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
    delay(100000000); // Delay to simulate some initialization time
    paging_init();
    print_string("Paging Initialized\n");
    delay(100000000); // Delay to simulate some initialization time
    interrupt_init();
    print_string("Interrupts Initialized\n");
    delay(100000000); // Delay to simulate some initialization time

    print_string("Kernel Initialized\n");
    clear_screen();
}


void kernel_main() {
    kernel_init();

    // Type any key to continue
    print_string("Interrupts enabled.\n");
    print_string("Press any key...\n");

    char c = keyboard_get_char();  // now truly interrupt‑driven
    print_string("You pressed: ");
    print_char(c);
    print_string("\n");

    // spin using hlt
    while (1) {
        __asm__ volatile("hlt");
    }
}
    


