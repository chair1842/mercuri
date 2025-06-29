// driver/pit.c
#include <stdint.h>
#include <io.h>           // inb/outb/io_wait
#include <driver/pit.h>
#include <interrupts/pic.h>
#include <interrupts/idt.h>

static volatile uint32_t ticks = 0;

// This is called by your central IRQ dispatcher via IRQ stub 0 (vector 32)
void pit_handler(void) {
    ticks++;
    pic_send_eoi(0);      // Acknowledge IRQ0 to the PIC
}

// Install the PIT handler and program the timer
void pit_init(uint32_t frequency_hz) {
    // 1) Hook the ISR stub in the IDT at vector 32
    extern void irq_stub_0();  // from your NASM: irq_stub 0
    idt_set_descriptor(32, irq_stub_0, 0x8E);

    // 2) Unmask IRQ0 in the PIC
    pic_clear_mask(0);

    // 3) Program PIT channel 0 for square wave: 
    //    Mode 3, LSB/MSB, channel 0 (port 0x43), divisor = base_freq / freq.
    uint32_t divisor = 1193182 / frequency_hz;
    outb(0x43, 0x36);           // Command port: channel 0, LSB/MSB, Mode 3
    io_wait();
    outb(0x40, divisor & 0xFF); // Divisor low byte
    io_wait();
    outb(0x40, (divisor >> 8) & 0xFF); // Divisor high byte
}

// Read the tick count
uint32_t pit_get_ticks(void) {
    return ticks;
}
