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

thread_t t1, t2;

void task1(void) {
    while (1) {
        print_string("A");
        for (volatile int i = 0; i < 1000000; i++);
    }
}

void task2(void) {
    while (1) {
        print_string("B");
        for (volatile int i = 0; i < 1000000; i++);
    }
}

void kernel_main() {
    pic_remap(0x20, 0x28);
    idt_init();
    pit_init(100);

    scheduler_init();
    thread_create(&t1, task1);
    thread_create(&t2, task2);

    clear_screen();
    
    thread_yield();

    for (;;) { __asm__ volatile("hlt"); }
}
    


