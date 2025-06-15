// driver/keyboard.c
// -----------------
// Single‐file, interrupt‐driven keyboard driver for polling + IRQ1

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

/* Special key codes (>=0x80, non-ASCII) */
#define KEY_ARROW_UP     0x80
#define KEY_ARROW_DOWN   0x81
#define KEY_ARROW_LEFT   0x82
#define KEY_ARROW_RIGHT  0x83

/* I/O ports */
#define DATA_PORT        0x60
#define STATUS_PORT      0x64
#define PIC1_DATA_PORT   0x21

/* Internal buffer */
#define BUFFER_SIZE      128
static char  buffer[BUFFER_SIZE];
static int   buffer_head = 0;
static int   buffer_tail = 0;

/* Modifier / state flags */
static bool shift_pressed       = false;
static bool ctrl_pressed        = false;
static bool alt_pressed         = false;
static bool caps_lock           = false;
static bool extended_scancode   = false;

/* Scancode→ASCII tables */
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

/* Port I/O helpers */
/* static inline uint8_t inb(uint16_t port) {
    uint8_t val;
    __asm__ volatile("inb %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
} */

/* Circular buffer operations */
static inline void buffer_push(char c) {
    int next = (buffer_head + 1) % BUFFER_SIZE;
    if (next != buffer_tail) {
        buffer[buffer_head] = c;
        buffer_head = next;
    }
}
static inline bool buffer_empty(void) {
    return buffer_head == buffer_tail;
}
static inline char buffer_pop(void) {
    if (buffer_empty()) return 0;
    char c = buffer[buffer_tail];
    buffer_tail = (buffer_tail + 1) % BUFFER_SIZE;
    return c;
}

/* Called from IRQ1 handler or polled code to grab one scancode */
void keyboard_poll(void) {
    if (!(inb(STATUS_PORT) & 1)) return;
    uint8_t sc = inb(DATA_PORT);

    /* Extended scancode prefix (0xE0) */
    if (sc == 0xE0) {
        extended_scancode = true;
        return;
    }

    bool released = sc & 0x80;
    uint8_t code  = sc & 0x7F;

    /* Handle modifiers and locks */
    if (!extended_scancode) {
        switch (code) {
            case 0x2A: case 0x36: /* Shift */
                shift_pressed = !released; return;
            case 0x1D: /* Ctrl */
                ctrl_pressed = !released; return;
            case 0x38: /* Alt */
                alt_pressed = !released; return;
            case 0x3A: /* Caps Lock */
                if (!released) caps_lock = !caps_lock;
                return;
        }
    }

    /* On key release, drop extended flag and do nothing */
    if (released) {
        extended_scancode = false;
        return;
    }

    /* Extended keys (arrow keys) */
    if (extended_scancode) {
        extended_scancode = false;
        switch (code) {
            case 0x48: buffer_push(KEY_ARROW_UP);    return;
            case 0x50: buffer_push(KEY_ARROW_DOWN);  return;
            case 0x4B: buffer_push(KEY_ARROW_LEFT);  return;
            case 0x4D: buffer_push(KEY_ARROW_RIGHT); return;
            default: return;
        }
    }

    /* Regular printable character */
    char c = 0;
    if (code < 128) {
        bool upper = shift_pressed ^ caps_lock;
        c = upper 
            ? scancode_to_ascii_shift[code] 
            : scancode_to_ascii[code];
    }
    if (c) buffer_push(c);
}

/* Public API */

void keyboard_init(void) {
    /* 1) Drain any stale scancodes */
    while (inb(STATUS_PORT) & 1) {
        (void)inb(DATA_PORT);
    }
    buffer_head = buffer_tail = 0;

    /* 2) Unmask IRQ1 on master PIC (port 0x21, bit 1) */
    uint8_t mask = inb(PIC1_DATA_PORT);
    mask &= ~(1 << 1);
    outb(PIC1_DATA_PORT, mask);
}

bool keyboard_available(void) {
    /* IRQ handler or polling code must call keyboard_poll() */
    keyboard_poll();
    return !buffer_empty();
}

char keyboard_read_char(void) {
    /* non-blocking: poll then pop */
    keyboard_poll();
    return buffer_pop();
}

char keyboard_get_char(void) {
    /* blocking: wait until something’s in the buffer */
    while (!keyboard_available()) {
        /* optionally halt CPU to save power: */
        __asm__ volatile("hlt");
    }
    return buffer_pop();
}
