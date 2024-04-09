#include "cnrom.h"
#include "nrom.h"
#include "mapper.h"
#include <ppu/ppu_memory.h>

using namespace mnes;
using namespace mnes::mappers;

namespace {
size_t s_nOffset;
auto &romData = current_rom_data();
auto &format = current_nes_format();
auto &ptable = ppu::memory::pattern_table();

namespace cnrom {
void write(uint32_t address, uint32_t value);
void reset();
void nop();
}

mapper_t this_mapper = {
    mapperNROM().read_memory, ppu_read_nop, cnrom::write, cnrom::nop, cnrom::nop, cnrom::reset, CNROM, false
};

}// namespace

mnes::mappers::mapper_t &mnes::mappers::mapperCNROM()
{
  return this_mapper;
}

namespace {
void cnrom::write(uint32_t address, uint32_t value)
{
  if (address < 0x8000) { return; }

  size_t nBankAddress = (value * 0x2000) + s_nOffset;

  auto rawData = current_raw_data();

  // ppu pages are 1k or 0x400
  for (size_t n = 0; n < 8; n++) { ptable[n] = rawData.subspan(nBankAddress + (0x400 * n)).data(); }
}

void cnrom::reset()
{
  auto rawData = current_raw_data();

  s_nOffset = 0x4000;
  if (format.prg_rom_count == 1) {
    // banks are 0x1000 in size
    romData[4] = romData[0] = rawData.subspan(0x0000).data();
    romData[5] = romData[1] = rawData.subspan(0x1000).data();
    romData[6] = romData[2] = rawData.subspan(0x2000).data();
    romData[7] = romData[3] = rawData.subspan(0x3000).data();
  } else {
    // 2
    s_nOffset = 0x8000;
    romData[0] = rawData.subspan(0x0000).data();
    romData[1] = rawData.subspan(0x1000).data();
    romData[2] = rawData.subspan(0x2000).data();
    romData[3] = rawData.subspan(0x3000).data();
    romData[4] = rawData.subspan(0x4000).data();
    romData[5] = rawData.subspan(0x5000).data();
    romData[6] = rawData.subspan(0x6000).data();
    romData[7] = rawData.subspan(0x7000).data();
  }

  // ppu pages are 1k or 0x400
  for (uint32_t n = 0; n < 8; n++) { ptable[n] = rawData.subspan(s_nOffset + (0x400 * n)).data(); }

  if ((format.rom_control_1 & 1) == 1) {
    SetVerticalMirror();
  } else {
    SetHorizontalMirror();
  }
}

void cnrom::nop() {}

}