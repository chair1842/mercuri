#ifndef CMOS_H
#define CMOS_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint8_t year;  // Only last two digits (e.g. 24 for 2024)
} cmos_time_t;

// Reads the current time into the provided struct
void cmos_get_time(cmos_time_t* time);

// Returns true if RTC is currently updating its values
bool cmos_is_updating(void);

// Converts BCD to binary (if needed manually)
uint8_t bcd_to_bin(uint8_t bcd);

void print_cmos_time(void);

#endif // CMOS_H
