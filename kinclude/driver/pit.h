#pragma once

#include <stdint.h>

void pit_handler();
uint32_t pit_get_ticks();
void pit_init(uint32_t frequency_hz);
