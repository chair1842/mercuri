#include <vga.h>
#include <stdint.h>
#include <interrupts/pic.h>
#include <io.h>
#include <driver/keyboard.h>

__attribute__((noreturn))
void exception_handler(int vector, int err_code);

__attribute__((noreturn))
void exception_handler(int vector, int err_code) {
    clear_screen();

    set_color(COLOR_LIGHT_RED, COLOR_BLACK);
    print_string("Exception!\n");
    print_string("Vector: ");
    print_int(vector);
    print_string("\nError Code: ");
    print_int(err_code);
    print_string("\nSystem halted\n");

    for (;;) __asm__ volatile("cli; hlt");
}

void irq_handler(uint32_t irq) {
    switch (irq) {
        case 0:
            print_string("Timer IRQ\n");
            break;
        case 1:
            keyboard_poll();
            break;
        default:
            print_string("IRQ ");
            print_int(vector);
            print_string("\n");
            break;
    }
    pic_send_eoi(irq + 32);
}