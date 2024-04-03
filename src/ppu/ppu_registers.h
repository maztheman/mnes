#pragma once

namespace mnes::ppu {
//public ppu register functions
uint32_t scanline();
void set_last_r2002_read(uint32_t value);

}
