#include <vga.h>
#include <string.h>

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

void input_command(const char* command) {
    if (command[0] == '\0') return; // Ignore empty commands

    if (strcmp(command, "help") == 0) {
        echo("Available commands:\n");
        echo("  help - Show this help message\n");
        echo("  echo <text> - Echo the text back\n");
        echo("  clear - Clear the screen\n");
    } else if (strncmp(command, "echo ", 5) == 0) {
        echo(command + 5); // Echo the text after 'echo '
    } else if (strcmp(command, "clear") == 0) {
        clear_screen();
        shell_welcome();
    } else {
        echo("Unknown command: ");
        echo(command);
        echo("\n");
    }
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