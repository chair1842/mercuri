#define CMOS_ADDRESS  0x70
#define CMOS_DATA     0x71

#define CMOS_SECONDS     0x00
#define CMOS_MINUTES     0x02
#define CMOS_HOURS       0x04
#define CMOS_DAY         0x07
#define CMOS_MONTH       0x08
#define CMOS_YEAR        0x09
#define CMOS_STATUS_B    0x0B

#include <io.h>
#include <stdint.h>
#include <stdbool.h>
#include <driver/cmos.h>
#include <vgatxm.h>

void delay(uint32_t count) {
    for (volatile uint32_t i = 0; i < count; i++);
}

uint8_t cmos_read(uint8_t reg) {
    outb(CMOS_ADDRESS, reg);
    return inb(CMOS_DATA);
}

bool cmos_is_updating() {
    outb(CMOS_ADDRESS, 0x0A);
    return inb(CMOS_DATA) & 0x80;
}

uint8_t bcd_to_bin(uint8_t val) {
    return ((val / 16) * 10) + (val & 0x0F);
}

void print_cmos_time() {
    cmos_time_t time;
    cmos_get_time(&time);
    
    // Print the time in a human-readable format
    /* print_string("Current CMOS Time:\n");
    print_int(time.year + 2000); // Assuming year is in 2000s
    print_string("-");
    print_int(time.month);
    print_string("-");
    print_int(time.day);
    print_string(" "); */
    print_int(time.hour);
    print_string(":");
    print_int(time.minute);
    print_string(":");
    print_int(time.second);
    print_string("\n");
}

void cmos_get_time(cmos_time_t* time) {
    // Wait until RTC is not updating
    while (cmos_is_updating());

    time->second = cmos_read(CMOS_SECONDS);
    delay(10000000);
    time->minute = cmos_read(CMOS_MINUTES);
    delay(10000000);
    time->hour   = cmos_read(CMOS_HOURS);
    delay(10000000);
    time->day    = cmos_read(CMOS_DAY);
    delay(10000000);
    time->month  = cmos_read(CMOS_MONTH);
    delay(10000000);
    time->year   = cmos_read(CMOS_YEAR);
    delay(10000000);

    // Check if values are BCD
    uint8_t status_b = cmos_read(CMOS_STATUS_B);
    delay(10000000);
    if (!(status_b & 0x04)) {
        // Convert from BCD to binary
        time->second = bcd_to_bin(time->second);
        time->minute = bcd_to_bin(time->minute);
        time->hour   = bcd_to_bin(time->hour);
        time->day    = bcd_to_bin(time->day);
        time->month  = bcd_to_bin(time->month);
        time->year   = bcd_to_bin(time->year);
    }

    // Optionally: convert 12-hour to 24-hour format
    if (!(status_b & 0x02) && (time->hour & 0x80)) {
        time->hour = ((time->hour & 0x7F) + 12) % 24;
    }
    print_cmos_time();
}