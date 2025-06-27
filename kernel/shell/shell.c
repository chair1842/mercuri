#include <vga.h>
#include <driver/keyboard.h>
#include <string.h>
#include "mprog/calculator.c"

#define CMD_BUF_SIZE 128

// Function declarations
void delay(volatile int count);
void shell_welcome();
void echo(const char* str);
void input_command(const char* command);
void shell();
void shell_init();

void delay(volatile int count) {
    for (volatile int i = 0; i < count; i++);
}

void shell_welcome() {
    clear_screen();
    set_color(COLOR_LIGHT_RED, COLOR_BLACK);
    print_string("MERCURI V1\nType 'help' for commands\n\n");
    set_color(COLOR_WHITE, COLOR_BLACK);
}

void echo(const char* str) {
    // print a newline, then the string
    print_char('\n');
    print_string(str);
}

// your existing command handler
void input_command(const char* command) {
    set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    if (command[0] == '\0') return;

    if (strcmp(command, "help") == 0) {
        echo("Available commands:");
        echo("  help         - Show this help message");
        echo("  echo <text>  - Echo the text back");
        echo("  clear        - Clear the screen");
        echo("  calc <expr>  - Calculate an expression (e.g., calc 2+3)");
    }
    else if (strncmp(command, "echo ", 5) == 0) {
        echo(command + 5);
    }
    else if (strcmp(command, "clear") == 0) {
        shell_init();
    }
    else if (strncmp(command, "calc", 4) == 0) {
    calc_command(command + 4);
    }
    else {
        echo("Unknown command: ");
        echo(command);
    }
}

void shell() {
    char buf[CMD_BUF_SIZE];
    int  len = 0;

    while (true) {
        // Print prompt
        set_color(COLOR_WHITE, COLOR_BLACK);
        print_string("\n\n# ");
        len = 0;
        // Read a line
        while (true) {
            // Wait for a key
            while (!keyboard_available());
            char c = keyboard_read_char();

            if (c == '\n' || c == '\r') {
                // Enter: terminate string and break
                buf[len] = '\0';
                break;
            }
            else if (c == '\b') {
                // Backspace
                if (len > 0) {
                    len--;
                    // Move cursor back, overwrite, move back
                    print_char('\b');
                    print_char(' ');
                    print_char('\b');
                }
            }
            else if (c >= ' ' && len < CMD_BUF_SIZE - 1) {
                // Printable character
                buf[len++] = c;
                print_char(c);
            }
            // ignore other keys
        }

        // Process the command
        input_command(buf);
    }
}

void shell_init() {
    shell_welcome();
    shell();
}
