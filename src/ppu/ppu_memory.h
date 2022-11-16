#pragma once

uint32_t ppu_memory_main_read(uint32_t address);
void ppu_memory_main_write(uint32_t address, uint32_t value);

std::array<uint8_t*, 8>& PPUTable();
std::array<uint8_t*, 4>& Tables();
std::span<uint8_t> Palette();
std::span<uint8_t> NTRam();
