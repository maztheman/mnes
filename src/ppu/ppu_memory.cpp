#include "ppu_memory.h"

#include <cpu/memory_registers.h>

#include <array>
#include <span>

using namespace mnes::ppu::memory;

namespace {
struct ppu_memory_t
{
  std::array<uint8_t *, 8> PPUTable = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
  std::array<uint8_t *, 4> Tables = { nullptr, nullptr, nullptr, nullptr };
  std::unique_ptr<uint8_t[]> VRAM = std::make_unique<uint8_t[]>(VRAM_SIZE);

  vram_memory_t VRAMSPAN() { return vram_memory_t(VRAM.get(), VRAM_SIZE); }

  palette_memory_t Palette() { return palette_memory_t(VRAMSPAN().subspan(PALETTE_OFFSET, PALETTE_SIZE)); }

  ntram_memory_t NTRam() { return ntram_memory_t(VRAMSPAN().subspan(NTRAM_OFFSET, NTRAM_SIZE)); }
};

ppu_memory_t &ppu_memory()
{
  static ppu_memory_t instance;
  return instance;
}
}

std::array<uint8_t *, 8> & mnes::ppu::memory::PPUTable() { return ppu_memory().PPUTable; }

std::array<uint8_t *, 4> & mnes::ppu::memory::Tables() { return ppu_memory().Tables; }

palette_memory_t mnes::ppu::memory::Palette() { return ppu_memory().Palette(); }

ntram_memory_t mnes::ppu::memory::NTRam() { return ppu_memory().NTRam(); }

namespace {
auto &ppuTable = PPUTable();
auto &tables = Tables();
}

uint mnes::ppu::memory::read(uint address)
{
  // cmem::set_ppu_addr_bus(address);

  address &= 0x3FFF;

  mappers::current()->read_ppu_memory(address);// allow mapper to read shit man

  // Pattern Table
  if (address < 0x2000) { return ppuTable[address >> 0xA][address & 0x3FF]; }

  // Name Table
  if (address < 0x3F00) {
    uint index = (address & 0xC00) >> 10;
    return tables[index][address & 0x3FF];
  }

  address &= 0x1F;
  return ppu_memory().Palette()[address];
}

void mnes::ppu::memory::write(uint address, uint value)
{
  // cmem::set_ppu_addr_bus(address);

  address &= 0x3FFF;

  // Pattern Table
  if (address < 0x2000) {
    ppu_memory().PPUTable[address >> 0xAU][address & 0x3FF] = TO_BYTE(value);
    return;
  }

  // Name Table
  if (address < 0x3F00) {
    size_t index = (address & 0xC00) >> 10;
    ppu_memory().Tables[index][address & 0x3FF] = TO_BYTE(value);
    return;
  }

  // Palette
  uint tmp = address & 0x1F;
  uint val = value & 0x3F;

  if (auto palette = ppu_memory().Palette(); tmp <= 0x10) {
    tmp &= 0xF;
    palette[tmp] = TO_BYTE(val);
    palette[0xC] = palette[0x8] = palette[0x4] = palette[0x0];
    palette[0x10] = palette[0x1C] = palette[0x18] = palette[0x14] = palette[0x0];
  } else {
    palette[tmp] = TO_BYTE(val);
  }
}