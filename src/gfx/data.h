#pragma once

#include <cstdint>
#include <span>

namespace mnes::gfx {
constexpr auto SCREEN_DATA_SIZE = 256U * 256U * 3U;
std::span<uint8_t, SCREEN_DATA_SIZE> screen_data();
void update_texture_from_screen_data();
void clear_off_screen_data();
}