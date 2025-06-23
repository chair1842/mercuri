// pic.c
#include <pic.h>
#include <io.h>  // your outb(), inb(), and io_wait()

// Remap PIC so IRQs 0–7 → vectors offset1..offset1+7,
//                8–15 → offset2..offset2+7
void pic_remap(int offset1, int offset2) {
    // Start initialization
    outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);
    io_wait();

    // Set vector offsets
    outb(PIC1_DATA, offset1);
    io_wait();
    outb(PIC2_DATA, offset2);
    io_wait();

    // Tell Master about Slave on IRQ2 (0000 0100)
    outb(PIC1_DATA, 4);
    io_wait();
    // Tell Slave its cascade identity (0000 0010)
    outb(PIC2_DATA, 2);
    io_wait();

    // Set mode to 8086/88
    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    // Mask all IRQs by default (you can unmask later)
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}

// Send End-Of-Interrupt for given IRQ line
void pic_send_eoi(uint8_t irq) {
    if (irq >= 8) {
        outb(PIC2_CMD, PIC_EOI);
    }
    outb(PIC1_CMD, PIC_EOI);
}

// Mask (disable) a single IRQ line
void pic_set_mask(uint8_t irq_line) {
    uint16_t port;
    uint8_t value;
    if (irq_line < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq_line -= 8;
    }
    value = inb(port) | (1 << irq_line);
    outb(port, value);
}

// Unmask (enable) a single IRQ line
void pic_clear_mask(uint8_t irq_line) {
    uint16_t port;
    uint8_t value;
    if (irq_line < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq_line -= 8;
    }
    value = inb(port) & ~(1 << irq_line);
    outb(port, value);
}
