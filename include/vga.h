#ifndef VGA_H
#define VGA_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// VGA display size
#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define VIDEO_MEMORY (volatile char*)0xB8000
// VGA color constants (0–15)
typedef enum {
    COLOR_BLACK         = 0x0,
    COLOR_BLUE          = 0x1,
    COLOR_GREEN         = 0x2,
    COLOR_CYAN          = 0x3,
    COLOR_RED           = 0x4,
    COLOR_MAGENTA       = 0x5,
    COLOR_BROWN         = 0x6,
    COLOR_LIGHT_GREY    = 0x7,
    COLOR_DARK_GREY     = 0x8,
    COLOR_LIGHT_BLUE    = 0x9,
    COLOR_LIGHT_GREEN   = 0xA,
    COLOR_LIGHT_CYAN    = 0xB,
    COLOR_LIGHT_RED     = 0xC,
    COLOR_LIGHT_MAGENTA = 0xD,
    COLOR_YELLOW        = 0xE,
    COLOR_WHITE         = 0xF,
} VGAColor;
static uint16_t cursor_row, cursor_col;
extern uint16_t input_start_row, input_start_col;



// Public API
void set_color(uint8_t fg, uint8_t bg);
void clear_screen();
void print_char(char c);
void print_string(const char* str);
void vga_set_cursor(uint16_t row, uint16_t col);
void vga_hide_cursor();
void vga_show_cursor();

#endif // VGA_H
