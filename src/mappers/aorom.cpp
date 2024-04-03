#include "aorom.h"
#include "nrom.h"

#include "mapper.h"

#include <common/Log.h>
#include <cpu/cpu_registers.h>
#include <cpu/memory.h>
#include <ppu/ppu_memory.h>
#include <ppu/ppu_registers.h>

#include <cstring>

using namespace mnes::ppu::memory;
using namespace mnes::mappers;
using mnes::memory::SRam;

namespace {
bool s_bSaveRam = false;
std::vector<uint8_t> s_arVRAM;
uint s_n32kBankMask;

void aorom_reset();
void aorom_write(uint address, uint value);
void aorom_nop();
}// namespace

mnes::mappers::mapper_t &mnes::mappers::mapperAOROM()
{
  static mapper_t instance = {
    mapperNROM().read_memory, ppu_read_nop, aorom_write, aorom_nop, aorom_nop, aorom_reset, AOROM, false
  };

  return instance;
}

namespace {
void aorom_write(uint address, uint value)
{
  static auto &tables = Tables();
  static auto ntram = NTRam();
  static auto &romData = current_rom_data();

  auto rawData = current_raw_data();

  if (address >= 0x6000 && address < 0x8000) {
    if (s_bSaveRam) { SRam()[address & 0x1FFF] = value & 0xFF; }
  }

  if (address < 0x8000) { return; }


  if ((value & 0x10) == 0x10) {
    tables[0] = tables[1] = tables[2] = tables[3] = ntram.subspan(0x400).data();
  } else {
    tables[0] = tables[1] = tables[2] = tables[3] = ntram.data();
  }

  uint n32kBank = ((value & 0xF) & s_n32kBankMask) * 0x8000;

  for (uint n = 0; n < 8; n++) { romData[n] = rawData.subspan(n32kBank + (0x1000 * n)).data(); }
}

void aorom_nop() {}

void aorom_reset()
{
  static auto &pputable = PPUTable();
  static auto &romData = current_rom_data();
  auto &format = current_nes_format();
  auto rawData = current_raw_data();

  for (uint n = 0; n < 8; n++) { romData[n] = rawData.subspan(0x1000 * n).data(); }

  s_arVRAM.resize(0x2000);
  for (uint n = 0; n < 8; n++) { pputable[n] = &s_arVRAM[(0x400 * n)]; }

  s_n32kBankMask = (format.prg_rom_count * 8) - 1U;

  if ((format.rom_control_1 & 8) == 8) {
    // don't do this
    //		SetFourScreenMirror();
  } else if ((format.rom_control_1 & 1) == 1) {
    SetVerticalMirror();
  } else {
    SetHorizontalMirror();
  }

  s_bSaveRam = mapperAOROM().m_bSaveRam = (format.rom_control_1 & 2) == 2;
}
}// namespace