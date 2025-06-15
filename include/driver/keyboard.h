#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdbool.h>
#include <stdint.h>

// Special key codes (outside ASCII range)
#define KEY_ARROW_UP     0x80
#define KEY_ARROW_DOWN   0x81
#define KEY_ARROW_LEFT   0x82
#define KEY_ARROW_RIGHT  0x83

void keyboard_init(void);
bool keyboard_available(void);
char keyboard_read_char(void);
char keyboard_get_char(void);
void keyboard_poll(void);

// Optional accessors
bool keyboard_shift_pressed(void);
bool keyboard_ctrl_pressed(void);
bool keyboard_alt_pressed(void);
bool keyboard_caps_lock(void);

#endif // KEYBOARD_H
