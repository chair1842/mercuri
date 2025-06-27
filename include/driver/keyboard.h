#pragma once

#include <stdbool.h>

#define KEY_ARROW_UP    0x80
#define KEY_ARROW_DOWN  0x81
#define KEY_ARROW_LEFT  0x82
#define KEY_ARROW_RIGHT 0x83

static int buffer_head;
static int buffer_tail;

void keyboard_init();
bool keyboard_available();
char keyboard_read_char();
char keyboard_get_char();
void keyboard_poll();