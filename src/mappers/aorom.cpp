#include "aorom.h"
#include "nrom.h"

#include "mapper.h"

#include <common/Log.h>
#include <cpu/cpu_registers.h>
#include <cpu/memory.h>
#include <ppu/ppu_memory.h>
#include <ppu/ppu_registers.h>

#include <cstring>

using namespace mnes;
using namespace mnes::mappers;
using mnes::memory::SRam;

namespace {
bool s_bSaveRam = false;
uint32_t s_n32kBankMask;
auto &ptable = ppu::memory::pattern_table();
auto &ntable = ppu::memory::name_table();
auto ntram = ppu::memory::NTRam();
auto &romData = current_rom_data();
auto &format = current_nes_format();
cart_vram_t vram = cart_vram();


namespace aorom {
  void reset();
  void write(uint32_t address, uint32_t value);
  void nop();
}

mapper_t this_mapper = {
    mapperNROM().read_memory, ppu_read_nop, aorom::write, aorom::nop, aorom::nop, aorom::reset, AOROM, false
};

}// namespace

mnes::mappers::mapper_t &mnes::mappers::mapperAOROM()
{
  return this_mapper;
}

namespace {
void aorom::write(uint32_t address, uint32_t value)
{
  auto rawData = current_raw_data();

  if (address >= 0x6000 && address < 0x8000) {
    if (s_bSaveRam) { SRam()[address & 0x1FFF] = value & 0xFF; }
  }

  if (address < 0x8000) { return; }

  // select name table
  if ((value & 0x10) == 0x10) {
    ntable[0] = ntable[1] = ntable[2] = ntable[3] = ntram.subspan(0x400).data();
  } else {
    ntable[0] = ntable[1] = ntable[2] = ntable[3] = ntram.data();
  }

  uint32_t n32kBank = ((value & 0xF) & s_n32kBankMask) * 0x8000;

  for (uint32_t n = 0; n < 8; n++) { romData[n] = rawData.subspan(n32kBank + (0x1000 * n)).data(); }
}

void aorom::nop() {}

void aorom::reset()
{
  auto rawData = current_raw_data();

  for (uint32_t n = 0; n < 8; n++) { romData[n] = rawData.subspan(0x1000 * n).data(); }

  for (uint32_t n = 0; n < 8; n++) { ptable[n] = vram.subspan(0x400 * n).data(); }

  s_n32kBankMask = (format.prg_rom_count * 8) - 1U;

  if ((format.rom_control_1 & 8) == 8) {
    // don't do this
    //		SetFourScreenMirror();
  } else if ((format.rom_control_1 & 1) == 1) {
    SetVerticalMirror();
  } else {
    SetHorizontalMirror();
  }

  s_bSaveRam = this_mapper.m_bSaveRam = (format.rom_control_1 & 2) == 2;
}
}// namespace