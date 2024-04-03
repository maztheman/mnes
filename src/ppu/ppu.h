#pragma once

namespace mnes::ppu {
void do_cycle();
void initialize();
bool is_odd_cycle();
void reset();
uint32_t get_cycle();
uint32_t get_current_scanline_cycle();
}