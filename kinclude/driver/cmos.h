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
    uint8_t year; // Note: only last 2 digits
} cmos_time_t;

uint8_t cmos_read(uint8_t reg);
bool cmos_is_updating(void);
void cmos_get_time(cmos_time_t* time);

#endif // CMOS_H
