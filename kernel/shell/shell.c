#include <vga.h>

void delay(volatile int count) {
    for (volatile int i = 0; i < count; i++);
}

void shell_welcome() {
    clear_screen();
    set_color(COLOR_LIGHT_RED, COLOR_DARK_GREY);
    print_string("MERCURI V1\nType 'help' for commands\n\n");
    delay(200000000);
    set_color(COLOR_WHITE, COLOR_BLACK);
}

void echo(const char* str) {
    set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    print_string(str);
    set_color(COLOR_WHITE, COLOR_BLACK);
}

void shell() {
    while (1) {
        print_string("#");
        delay(10000000);
    }
}

void shell_init() {
    shell_welcome();
    shell();
}