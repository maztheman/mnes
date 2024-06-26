#include "nrom.h"

#include "mapper.h"

#include <cpu/memory.h>
#include <ppu/ppu_memory.h>


namespace {
static std::vector<unsigned char> s_arVRAM;

namespace nrom {
uint read(uint address);
void write(uint, uint);
void nop();
void reset();
}
}

using namespace mnes::mappers;
using mnes::memory::SRam;
using namespace mnes::ppu::memory;

mnes::mappers::mapper_t &mnes::mappers::mapperNROM()
{
  static mapper_t instance = {
    nrom::read, ppu_read_nop, nrom::write, nrom::nop, nrom::nop, nrom::reset, NROM, false
  };
  return instance;
}

namespace {
uint nrom::read(uint address)
{
  static auto &romData = current_rom_data();

  if (address >= 0x6000 && address < 0x8000) {
    if (current()->m_bSaveRam) { return SRam()[address & 0x1FFF]; }
  }

  if (address >= 0x8000 && address <= 0xFFFF) {
    uint bank = (address >> 0xCU) - 8U;
    return romData[bank][address & 0xFFF];
  }

  return 0;
}

void nrom::write(uint, uint) {}

void nrom::nop() {}

void nrom::reset()
{
  static auto &romData = current_rom_data();
  auto &format = current_nes_format();
  auto rawData = current_raw_data();

  printf("Mapper[nrom]:reset()\n");
  uint nOffset = 0x4000;
  if (format.prg_rom_count == 1) {
    // banks are 0x1000 in size
    romData[4] = romData[0] = rawData.subspan(0x0000).data();
    romData[5] = romData[1] = rawData.subspan(0x1000).data();
    romData[6] = romData[2] = rawData.subspan(0x2000).data();
    romData[7] = romData[3] = rawData.subspan(0x3000).data();
  } else {// 2
    nOffset = 0x8000;
    romData[0] = rawData.subspan(0x0000).data();
    romData[1] = rawData.subspan(0x1000).data();
    romData[2] = rawData.subspan(0x2000).data();
    romData[3] = rawData.subspan(0x3000).data();
    romData[4] = rawData.subspan(0x4000).data();
    romData[5] = rawData.subspan(0x5000).data();
    romData[6] = rawData.subspan(0x6000).data();
    romData[7] = rawData.subspan(0x7000).data();
  }

  auto &ppuTable = PPUTable();

  if (format.chr_rom_count == 0) {
    s_arVRAM.resize(0x2000, 0);
    // ppu pages are 1k or 0x400
    for (uint n = 0; n < 8; n++) { ppuTable[n] = &s_arVRAM[(0x400 * n)]; }
  } else {
    // ppu pages are 1k or 0x400
    for (uint n = 0; n < 8; n++) { ppuTable[n] = rawData.subspan(nOffset + (0x400 * n)).data(); }
  }

  if ((format.rom_control_1 & 1) == 1) {
    SetVerticalMirror();
  } else {
    SetHorizontalMirror();
  }
}
}