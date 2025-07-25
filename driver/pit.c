// driver/pit.c
#include <stdint.h>
#include <io.h>           // inb/outb/io_wait
#include <driver/pit.h>
#include <interrupts/pic.h>
#include <interrupts/idt.h>
#include <vgatxm.h>


static volatile uint32_t ticks = 0;

// Install the PIT handler and program the timer
void pit_init(uint32_t frequency_hz) {
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

// This is called by your central IRQ dispatcher via IRQ stub 0 (vector 32)
void pit_handler(void) {
    ticks++;
}