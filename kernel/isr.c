#include <vga.h>

void isr_handler_c() {
    // You can read the interrupt number from stack,
    // but for now just print a dot or number
    print_char('*');
}
