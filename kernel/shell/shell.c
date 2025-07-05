#include <vga.h>
#include <driver/keyboard.h>
#include <string.h>
#include "mprog/calculator.c"
#include <driver/pit.h>

#define CMD_BUF_SIZE 128

// Function declarations
void delay(volatile int count);
void shell_welcome();
void input_command(const char* command);
void shell();
void shell_init();

void delay(volatile int count) {
    for (volatile int i = 0; i < count; i++);
}

void shell_welcome() {
    clear_screen();
    set_color(COLOR_LIGHT_RED, COLOR_BLACK);
    print_string("MERCURI V1.3\nType 'help' for commands\n\n");
    set_color(COLOR_WHITE, COLOR_BLACK);
}

// your existing command handler
void input_command(const char* command) {
    set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    if (command[0] == '\0') return;

    if (strcmp(command, "help") == 0) {
        print_string("Available commands:\n");
        print_string("  help         - Show this help message\n");
        print_string("  echo <text>  - Echo the text back\n");
        print_string("  clear        - Clear the screen\n");
        print_string("  calc <expr>  - Calculate an expression (e.g., calc 2+3)\n");
        print_string("  about        - Show the about message\n");
    }
    else if (strncmp(command, "echo ", 5) == 0) {
        print_string(command + 5);
    }
    else if (strcmp(command, "clear") == 0) {
        shell_init();
    }
    else if (strncmp(command, "calc", 4) == 0) {
        calc_command(command + 4);
    }
    else if (strcmp(command, "about") == 0) {
        print_string("About Mercuri V1.3\nBy Chair1842\nFrom June 2025");
    }
    else {
        print_string("Unknown command: ");
        print_string(command);
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
        print_char('\n');
        input_command(buf);
    }
}

void shell_init() {
    shell_welcome();
    shell();
}
