#include <vgatxm.h>
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
    print_string("MERCURI V1.4\nType 'help' for commands\n\n");
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
        print_string("  secfm        - Show how many seconds have passed since boot\n");
        print_string("  minfm        - Show how many minutes have passed since boot\n");
        //print_string("  hrfm         - Show how many hours have passed since boot\n");
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
        print_string("About Mercuri V1.4.1\nBy Chair1842\nFrom July 2025\nNew in 1.4.1: MA9Y command that will corrupt the os, tested in QEMU");
    }
    else if (strcmp(command, "secfm") == 0) {
        print_int(pit_get_ticks() / 100);
    }
    else if (strcmp(command, "minfm") == 0) {
        print_int(pit_get_ticks() / 6000);
    }
    /*else if (strcmp(command, "hrfm") == 0) {
        print_int(pit_get_ticks() / 64800);
    } */
   else if (strcmp(command, "MA9Y") == 0) {
        while (1) {
            print_string("CORRUPT");
        }
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
