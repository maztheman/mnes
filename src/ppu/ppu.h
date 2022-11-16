#pragma once

#include <functional>

#include "gfx/data.h"

void ppu_do_cycle();
void ppu_initialize();
bool ppu_is_odd_cycle();
void ppu_reset();
uint32_t ppu_get_cycle();
uint32_t ppu_get_current_scanline_cycle();
