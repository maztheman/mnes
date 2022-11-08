#pragma once

#include <span>
#include <cstdint>

static constexpr auto SCREEN_DATA_SIZE = 256U * 256U * 3;


std::span<uint8_t, SCREEN_DATA_SIZE> getScreenData();