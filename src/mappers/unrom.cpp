#include "unrom.h"

#include "nrom.h"

#include "mapper.h"

#include <ppu/ppu_memory.h>

#include <cstring>

using namespace mnes;
using namespace mnes::mappers;

namespace {
auto cvram = cart_vram();
auto &romData = current_rom_data();
auto &format = current_nes_format();
auto &ptable = ppu::memory::pattern_table();


namespace unrom {
void write(uint32_t address, uint32_t value);
void nop();
void reset();
}

mapper_t this_mapper = {
    mapperNROM().read_memory, ppu_read_nop, unrom::write, unrom::nop, unrom::nop, unrom::reset, UNROM, false
};
}

mnes::mappers::mapper_t & mnes::mappers::mapperUNROM()
{
  return this_mapper;
}

namespace {
void unrom::write(uint32_t address, uint32_t value)
{
  auto rawData = current_raw_data();

  if (address < 0x8000) { return; }

  uint32_t nBankAddress = value * 0x4000;

  romData[0] = rawData.subspan(nBankAddress + 0x0000).data();
  romData[1] = rawData.subspan(nBankAddress + 0x1000).data();
  romData[2] = rawData.subspan(nBankAddress + 0x2000).data();
  romData[3] = rawData.subspan(nBankAddress + 0x3000).data();
}

void unrom::nop() {}

void unrom::reset()
{
  auto rawData = current_raw_data();

  romData[0] = rawData.subspan(0x0000).data();
  romData[1] = rawData.subspan(0x1000).data();
  romData[2] = rawData.subspan(0x2000).data();
  romData[3] = rawData.subspan(0x3000).data();

  uint32_t nLastBank = (format.prg_rom_count - 1U) * 0x4000U;

  romData[4] = rawData.subspan(nLastBank + 0x0000).data();
  romData[5] = rawData.subspan(nLastBank + 0x1000).data();
  romData[6] = rawData.subspan(nLastBank + 0x2000).data();
  romData[7] = rawData.subspan(nLastBank + 0x3000).data();



  for (uint32_t n = 0; n < 8; n++) { ptable[n] = cvram.subspan(0x400 * n).data(); }

  if ((format.rom_control_1 & 1) == 1) {
    SetVerticalMirror();
  } else {
    SetHorizontalMirror();
  }
}
}