#include "ppu_memory.h"

#include <cpu/memory_registers.h>

#include <array>
#include <span>

using namespace mnes::ppu::memory;
using namespace mnes::ppu::memory::constants;

namespace {
constexpr auto VRAM_SIZE = NTRAM_OFFSET + NTRAM_SIZE;
using vram_memory_t = fixed_span_bytes<VRAM_SIZE>;

struct ppu_memory_t
{
  ppu_memory_t()
  : pattern_table({ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr })
  , name_table({ nullptr, nullptr, nullptr, nullptr })
  , ppu_vram(std::make_unique<uint8_t[]>(VRAM_SIZE))
  {
  }

  std::array<uint8_t *, 8> pattern_table;
  std::array<uint8_t *, 4> name_table;
  std::unique_ptr<uint8_t[]> ppu_vram;

  vram_memory_t ppu_vram_span() const { return vram_memory_t(ppu_vram.get(), VRAM_SIZE); }

  palette_memory_t palette_ram() const { return palette_memory_t(ppu_vram_span().subspan(PALETTE_OFFSET, PALETTE_SIZE)); }

  ntram_memory_t nt_ram() const {
    return ntram_memory_t(ppu_vram_span().subspan(NTRAM_OFFSET, NTRAM_SIZE));
  }
};

/// @brief create singleton that guarantees ppu_memory_t is fully initialized 
/// before its being used as a global variable everywhere
/// @return singleton instance
ppu_memory_t& singleton()
{
  static ppu_memory_t instance;
  return instance;
}
}

std::array<uint8_t *, 8> & mnes::ppu::memory::pattern_table() { return singleton().pattern_table; }

std::array<uint8_t *, 4> & mnes::ppu::memory::name_table() { return singleton().name_table; }

palette_memory_t mnes::ppu::memory::Palette() { return singleton().palette_ram(); }

ntram_memory_t mnes::ppu::memory::NTRam() { return singleton().nt_ram(); }

uint32_t mnes::ppu::memory::read(uint32_t address)
{
  address &= 0x3FFF;

  // Mappers have access to reads to ppu addr bus (eg. mmc5)
  mappers::current()->read_ppu_memory(address);

  // Pattern Table
  if (address < 0x2000) { return singleton().pattern_table[address >> 0xA][address & 0x3FF]; }

  // Name Table
  if (address < 0x3F00) {
    uint32_t index = (address & 0xC00) >> 10;
    return singleton().name_table[index][address & 0x3FF];
  }

  address &= 0x1F;
  return singleton().palette_ram()[address];
}

void mnes::ppu::memory::write(uint32_t address, uint32_t value)
{
  address &= 0x3FFF;

  // Pattern Table
  if (address < 0x2000) {
    singleton().pattern_table[address >> 0xAU][address & 0x3FF] = TO_BYTE(value);
    return;
  }

  // Name Table
  if (address < 0x3F00) {
    size_t index = (address & 0xC00) >> 10;
    singleton().name_table[index][address & 0x3FF] = TO_BYTE(value);
    return;
  }

  // Palette
  uint32_t tmp = address & 0x1F;
  uint32_t val = value & 0x3F;

  if (auto palette = singleton().palette_ram(); tmp <= 0x10) {
    tmp &= 0xF;
    palette[tmp] = TO_BYTE(val);
    palette[0xC] = palette[0x8] = palette[0x4] = palette[0x0];
    palette[0x10] = palette[0x1C] = palette[0x18] = palette[0x14] = palette[0x0];
  } else {
    palette[tmp] = TO_BYTE(val);
  }
}