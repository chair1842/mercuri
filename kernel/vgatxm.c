#include <stdint.h>
#include <stdbool.h>
#include <vgatxm.h>
#include <io.h>

static uint8_t current_color = 0x07;
static uint16_t cursor_row = 0, cursor_col = 0;
uint16_t input_start_row = 0;
uint16_t input_start_col = 0;

void set_color(uint8_t fg, uint8_t bg) {
    current_color = (bg << 4) | (fg & 0x0F);
}

void scroll() {
    volatile char* video_memory = VIDEO_MEMORY;

    for (uint16_t row = 1; row < VGA_HEIGHT; row++) {
        for (uint16_t col = 0; col < VGA_WIDTH; col++) {
            uint16_t from = (row * VGA_WIDTH + col) * 2;
            uint16_t to   = ((row - 1) * VGA_WIDTH + col) * 2;
            video_memory[to]     = video_memory[from];
            video_memory[to + 1] = video_memory[from + 1];
        }
    }

    // Clear last row
    for (uint16_t col = 0; col < VGA_WIDTH; col++) {
        uint16_t pos = ((VGA_HEIGHT - 1) * VGA_WIDTH + col) * 2;
        video_memory[pos]     = ' ';
        video_memory[pos + 1] = current_color;
    }

    cursor_row = VGA_HEIGHT - 1;
    cursor_col = 0;
}

void clear_screen() {
    volatile char* video_memory = VIDEO_MEMORY;
    for (uint16_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        video_memory[i * 2]     = ' ';
        video_memory[i * 2 + 1] = current_color;
    }
    cursor_row = 0;
    cursor_col = 0;
}

void print_char(char c) {
    volatile char* video_memory = VIDEO_MEMORY;

    if (c == '\n') {
        cursor_row++;
        cursor_col = 0;
    } else if (c == '\t') {
        cursor_col = (cursor_col + 4) & ~(4 - 1);
    } else if (c == '\b') {
        // Prevent backspace before input_start
        if (cursor_row < input_start_row ||
            (cursor_row == input_start_row && cursor_col <= input_start_col)) {
            return; // Don't delete before prompt
        }
        if (cursor_col > 0) {
            cursor_col--;
        } else if (cursor_row > 0) {
            cursor_row--;
            cursor_col = VGA_WIDTH - 1;
        }
        uint16_t pos = (cursor_row * VGA_WIDTH + cursor_col) * 2;
        video_memory[pos]     = ' ';
        video_memory[pos + 1] = current_color;
    } else {
        uint16_t pos = (cursor_row * VGA_WIDTH + cursor_col) * 2;
        video_memory[pos]     = c;
        video_memory[pos + 1] = current_color;
        cursor_col++;
        if (cursor_col >= VGA_WIDTH) {
            cursor_col = 0;
            cursor_row++;
        }
    }

    if (cursor_row >= VGA_HEIGHT) {
        scroll();
    }

    vga_set_cursor(cursor_row, cursor_col);
}

void print_string(const char* str) {
    for (uint16_t i = 0; str[i] != '\0'; i++) {
        print_char(str[i]);
    }
}

void print_int(int32_t value) {
    char buf[12];     // enough for “-2147483648\0”
    int  i = 0;
    bool neg = false;

    if (value == 0) {
        print_char('0');
        return;
    }

    if (value < 0) {
        neg = true;
        // handle INT32_MIN specially, since -INT32_MIN overflows
        if (value == INT32_MIN) {
            // "-2147483648"
            print_string("-2147483648");
            return;
        }
        value = -value;
    }

    // Extract digits in reverse order
    while (value > 0) {
        buf[i++] = '0' + (value % 10);
        value /= 10;
    }

    if (neg) {
        buf[i++] = '-';
    }

    // Now print in correct order
    while (i--) {
        print_char(buf[i]);
    }
}

#define VGA_PORT_COMMAND 0x3D4
#define VGA_PORT_DATA    0x3D5

void vga_set_cursor(uint16_t row, uint16_t col) {
    uint16_t position = row * VGA_WIDTH + col;

    outb(VGA_PORT_COMMAND, 0x0F);
    outb(VGA_PORT_DATA, (uint8_t)(position & 0xFF));

    outb(VGA_PORT_COMMAND, 0x0E);
    outb(VGA_PORT_DATA, (uint8_t)((position >> 8) & 0xFF));
}

void vga_hide_cursor() {
    outb(VGA_PORT_COMMAND, 0x0A);
    outb(VGA_PORT_DATA, 0x20); // Disable cursor (bit 5 = 1)
}

void vga_show_cursor() {
    // Typical values: cursor scanline start = 0, end = 15
    outb(VGA_PORT_COMMAND, 0x0A);
    outb(VGA_PORT_DATA, 0x00); // Bit 5 cleared → cursor enabled

    outb(VGA_PORT_COMMAND, 0x0B);
    outb(VGA_PORT_DATA, 0x0F); // 15th scanline = bottom of character cell
}
