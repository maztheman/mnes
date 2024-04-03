#pragma once

namespace mnes::ppu::memory {
static constexpr auto PALETTE_OFFSET = 0;
static constexpr auto PALETTE_SIZE = 64;
using palette_memory_t = fixed_span_bytes<PALETTE_SIZE>;
static constexpr auto NTRAM_OFFSET = PALETTE_OFFSET + PALETTE_SIZE;
static constexpr auto NTRAM_SIZE = 0x800;
using ntram_memory_t = fixed_span_bytes<NTRAM_SIZE>;
static constexpr auto VRAM_SIZE = NTRAM_OFFSET + NTRAM_SIZE;
using vram_memory_t = fixed_span_bytes<VRAM_SIZE>;

uint32_t read(uint32_t address);
void write(uint32_t address, uint32_t value);

std::array<uint8_t *, 8> &PPUTable();
std::array<uint8_t *, 4> &Tables();
palette_memory_t Palette();
ntram_memory_t NTRam();
}// namespace mnes::ppu::memory