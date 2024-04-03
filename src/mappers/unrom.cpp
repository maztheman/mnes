#include "unrom.h"

#include "nrom.h"

#include "mapper.h"

#include <ppu/ppu_memory.h>

#include <cstring>
#include <vector>

using namespace mnes::mappers;
using namespace mnes::ppu::memory;

namespace {
uint8_t s_VRAM[0x2000];

namespace unrom {
void write(uint address, uint value);
void nop();
void reset();
}
}

mnes::mappers::mapper_t & mnes::mappers::mapperUNROM()
{
  static mapper_t instance = {
    mapperNROM().read_memory, ppu_read_nop, unrom::write, unrom::nop, unrom::nop, unrom::reset, UNROM, false
  };

  return instance;
}

namespace {
void unrom::write(uint address, uint value)
{
  static auto &romData = current_rom_data();
  auto rawData = current_raw_data();

  if (address < 0x8000) { return; }

  uint nBankAddress = value * 0x4000;

  romData[0] = rawData.subspan(nBankAddress + 0x0000).data();
  romData[1] = rawData.subspan(nBankAddress + 0x1000).data();
  romData[2] = rawData.subspan(nBankAddress + 0x2000).data();
  romData[3] = rawData.subspan(nBankAddress + 0x3000).data();
}

void unrom::nop() {}

void unrom::reset()
{
  static auto &romData = current_rom_data();
  auto &format = current_nes_format();
  auto rawData = current_raw_data();

  romData[0] = rawData.subspan(0x0000).data();
  romData[1] = rawData.subspan(0x1000).data();
  romData[2] = rawData.subspan(0x2000).data();
  romData[3] = rawData.subspan(0x3000).data();

  uint nLastBank = (format.prg_rom_count - 1U) * 0x4000U;

  romData[4] = rawData.subspan(nLastBank + 0x0000).data();
  romData[5] = rawData.subspan(nLastBank + 0x1000).data();
  romData[6] = rawData.subspan(nLastBank + 0x2000).data();
  romData[7] = rawData.subspan(nLastBank + 0x3000).data();

  memset(&s_VRAM[0], 0, 0x2000);

  static auto &ppuTable = PPUTable();

  for (uint n = 0; n < 8; n++) { ppuTable[n] = &s_VRAM[(0x400 * n)]; }

  if ((format.rom_control_1 & 1) == 1) {
    SetVerticalMirror();
  } else {
    SetHorizontalMirror();
  }
}
}