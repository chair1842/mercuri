#include <vgatxm.h>
#include <driver/keyboard.h>
#include <memory/memory.h>
#include <memory/paging.h>
#include <interrupts/idt.h>
#include <interrupts/pic.h>
#include "shell/shell.c"
#include <driver/pit.h>
#include <threading/thread.h>

void kernel_init() {
    clear_screen();
    memory_init();
    print_string("Memory Initialized\n");
    delay(10000000);
    paging_init();
    print_string("Paging Initialized\n");
    delay(10000000);

    pic_remap(0x20, 0x28);
    print_string("PIC Remapped\n");
    delay(10000000);
    idt_init();
    print_string("IDT Initialized\n");
    delay(10000000);

    keyboard_init();
    print_string("Keyboard Initialized\n");
    delay(10000000);
    
    pit_init(100);
    print_string("PIT Initialized\n");
    delay(10000000);

    print_string("Kernel Initialized\n");
    delay(10000000);
    clear_screen();
}

void kernel_main() {
    kernel_init();
    shell_init();
}
    


