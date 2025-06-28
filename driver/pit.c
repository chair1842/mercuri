#include <stdint.h>
#include <io.h>
#include <driver/pit.h>

uint32_t ticks = 0;
void pit_handler() {
    ticks++;
    // Acknowledge the PIT by reading the status register
    inb(0x61);  // Read from port 0x61 to acknowledge
}