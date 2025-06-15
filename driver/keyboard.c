#include <stdint.h>
#include <stdbool.h>
#include <driver/keyboard.h>
#include <io.h>
#include <vga.h>

/* Public API */
void keyboard_init(void);
bool keyboard_available(void);
char keyboard_read_char(void);
char keyboard_get_char(void);
bool keyboard_debug = false;  // Toggle at runtime

#define KEY_ARROW_UP     0x80
#define KEY_ARROW_DOWN   0x81
#define KEY_ARROW_LEFT   0x82
#define KEY_ARROW_RIGHT  0x83

#define DATA_PORT        0x60
#define STATUS_PORT      0x64
#define PIC1_DATA_PORT   0x21

#define BUFFER_SIZE      128
static char  buffer[BUFFER_SIZE];
static int   buffer_head = 0;
static int   buffer_tail = 0;

static bool shift_pressed       = false;
static bool ctrl_pressed        = false;
static bool alt_pressed         = false;
static bool caps_lock           = false;
static bool extended_scancode   = false;

static const char scancode_to_ascii[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,'\\','z','x','c','v','b','n','m',',','.','/',
    0,'*', 0,' ', 0,
};
static const char scancode_to_ascii_shift[128] = {
    0, 27, '!','@','#','$','%','^','&','*','(',')','_','+','\b',
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
        if (keyboard_debug) {
            print_string("[kbd] Push: ");
            print_char(c);
            print_string("\n");
        }
    } else if (keyboard_debug) {
        print_string("[kbd] Buffer full, dropping char\n");
    }
}

static inline bool buffer_empty(void) {
    return buffer_head == buffer_tail;
}

static inline char buffer_pop(void) {
    if (buffer_empty()) return 0;
    char c = buffer[buffer_tail];
    buffer_tail = (buffer_tail + 1) % BUFFER_SIZE;
    if (keyboard_debug) {
        print_string("[kbd] Pop: ");
        print_char(c);
        print_string("\n");
    }
    return c;
}

void keyboard_poll(void) {
    if (!(inb(STATUS_PORT) & 1)) return;
    uint8_t sc = inb(DATA_PORT);

    if (keyboard_debug) {
        print_string("[kbd] Scancode: ");
        print_hex(sc);
        print_string("\n");
    }

    if (sc == 0xE0) {
        extended_scancode = true;
        if (keyboard_debug) print_string("[kbd] Extended code\n");
        return;
    }

    bool released = sc & 0x80;
    uint8_t code = sc & 0x7F;

    if (!extended_scancode) {
        switch (code) {
            case 0x2A: case 0x36:
                shift_pressed = !released;
                if (keyboard_debug) print_string("[kbd] Shift toggled\n");
                return;
            case 0x1D:
                ctrl_pressed = !released;
                if (keyboard_debug) print_string("[kbd] Ctrl toggled\n");
                return;
            case 0x38:
                alt_pressed = !released;
                if (keyboard_debug) print_string("[kbd] Alt toggled\n");
                return;
            case 0x3A:
                if (!released) {
                    caps_lock = !caps_lock;
                    if (keyboard_debug) print_string("[kbd] Caps Lock toggled\n");
                }
                return;
        }
    }

    if (released) {
        extended_scancode = false;
        return;
    }

    if (extended_scancode) {
        extended_scancode = false;
        switch (code) {
            case 0x48: buffer_push(KEY_ARROW_UP); return;
            case 0x50: buffer_push(KEY_ARROW_DOWN); return;
            case 0x4B: buffer_push(KEY_ARROW_LEFT); return;
            case 0x4D: buffer_push(KEY_ARROW_RIGHT); return;
            default:
                if (keyboard_debug) print_string("[kbd] Unknown extended key\n");
                return;
        }
    }

    if (code < 128) {
        bool upper = shift_pressed ^ caps_lock;
        char c = upper ? scancode_to_ascii_shift[code] : scancode_to_ascii[code];
        if (c) {
            buffer_push(c);
        } else if (keyboard_debug) {
            print_string("[kbd] Unknown printable key\n");
        }
    }
}

void keyboard_init(void) {
    while (inb(STATUS_PORT) & 1) {
        (void)inb(DATA_PORT);
    }
    buffer_head = buffer_tail = 0;

    uint8_t mask = inb(PIC1_DATA_PORT);
    mask &= ~(1 << 1);
    outb(PIC1_DATA_PORT, mask);

    if (keyboard_debug) print_string("[kbd] Keyboard initialized\n");
}

bool keyboard_available(void) {
    keyboard_poll();
    return !buffer_empty();
}

char keyboard_read_char(void) {
    keyboard_poll();
    return buffer_pop();
}

char keyboard_get_char(void) {
    while (!keyboard_available()) {
        __asm__ volatile("hlt");
    }
    return buffer_pop();
}
