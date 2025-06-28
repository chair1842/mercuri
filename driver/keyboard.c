#include <driver/keyboard.h>
#include <vga.h>
#include <stdbool.h>
#include <stdint.h>
#include <io.h>
#include <interrupts/pic.h>
#include <interrupts/idt.h>

#define DATA_PORT   0x60
#define STATUS_PORT 0x64
#define BUFFER_SIZE 128

static char buffer[BUFFER_SIZE];
static bool shift_pressed = false;
static bool ctrl_pressed = false;
static bool alt_pressed = false;
static bool caps_lock = false;
static bool extended_scancode = false;


static char scancode_to_ascii[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,'\\','z','x','c','v','b','n','m',',','.','/',
    0,'*', 0,' ', 0,
};

static char scancode_to_ascii_shift[128] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n',
    0,'A','S','D','F','G','H','J','K','L',':','"','~',
    0,'|','Z','X','C','V','B','N','M','<','>','?',
    0,'*', 0,' ', 0,
};

static inline void buffer_push(char c) {
    int next = (buffer_head + 1) % BUFFER_SIZE;
    if (next != buffer_tail) {
        buffer[buffer_head] = c;
        buffer_head = next;
    }
}

static inline bool buffer_empty() {
    return buffer_head == buffer_tail;
}

static inline char buffer_pop() {
    if (buffer_empty()) return 0;
    char c = buffer[buffer_tail];
    buffer_tail = (buffer_tail + 1) % BUFFER_SIZE;
    return c;
}

void keyboard_poll() {
    if (!(inb(STATUS_PORT) & 1)) return;
    uint8_t sc = inb(DATA_PORT);

    if (sc == 0xE0) {
        extended_scancode = true;
        return;
    }

    bool key_released = sc & 0x80;
    uint8_t code = sc & 0x7F;

    // Handle modifiers and toggle keys
    if (!extended_scancode) {
        switch (code) {
            case 0x2A: case 0x36: shift_pressed = !key_released; return;
            case 0x1D: ctrl_pressed = !key_released; return;
            case 0x38: alt_pressed = !key_released; return;
            case 0x3A: if (!key_released) caps_lock = !caps_lock; return;
        }
    }

    if (key_released) {
        extended_scancode = false;
        return;
    }

    // Handle extended keys
    if (extended_scancode) {
        extended_scancode = false;

        switch (code) {
            case 0x48: buffer_push(KEY_ARROW_UP); return;
            case 0x50: buffer_push(KEY_ARROW_DOWN); return;
            case 0x4B: buffer_push(KEY_ARROW_LEFT); return;
            case 0x4D: buffer_push(KEY_ARROW_RIGHT); return;
        }
        return; // Unrecognized extended
    }

    // Regular printable key
    char c = 0;
    if (code < 128) {
        if (shift_pressed ^ caps_lock) {
            c = scancode_to_ascii_shift[code];
        } else {
            c = scancode_to_ascii[code];
        }
    }

    if (c) buffer_push(c);
    inb(DATA_PORT);
}

bool keyboard_available() {
    return !buffer_empty();
}

char keyboard_read_char() {
    return buffer_pop();
}

char keyboard_get_char() {
    while (!keyboard_available());
    return buffer_pop();
}

void keyboard_init() {
    pic_clear_mask(1);

    // 2) Install the IRQ1 stub in your IDT (vector 33)
    extern void irq_stub_33();  // NASM stub: pushes 1 then calls irq_handler
    idt_set_descriptor(33, irq_stub_33, 0x8E);
}