#include <vga.h>
#include <stdint.h>

__attribute__((noreturn))
void exception_handler(int vector, int err_code);

__attribute__((noreturn))
void exception_handler(int vector, int err_code) {
    clear_screen();

    set_color(COLOR_LIGHT_RED, COLOR_BLACK);
    print_string("Exception!\n");
    print_string("System halted\n");

    for (;;) __asm__ volatile("cli; hlt");
}