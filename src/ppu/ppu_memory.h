#pragma once

namespace mnes::ppu::memory {
namespace constants {
static constexpr auto PALETTE_OFFSET = 0U;
static constexpr auto PALETTE_SIZE = 64U;
static constexpr auto NTRAM_OFFSET = PALETTE_OFFSET + PALETTE_SIZE;
static constexpr auto NTRAM_SIZE = 0x800U;
}
using palette_memory_t = fixed_span_bytes<constants::PALETTE_SIZE>;
using ntram_memory_t = fixed_span_bytes<constants::NTRAM_SIZE>;

uint32_t read(uint32_t address);
void write(uint32_t address, uint32_t value);

std::array<uint8_t *, 8> &pattern_table();
std::array<uint8_t *, 4> &name_table();
palette_memory_t Palette();
ntram_memory_t NTRam();
}// namespace mnes::ppu::memory