#include "aorom.h"
#include "nrom.h"

#include "mapper.h"

#include <common/Log.h>
#include <cpu/cpu_registers.h>
#include <cpu/memory.h>
#include <ppu/ppu_memory.h>
#include <ppu/ppu_registers.h>

#include <cstring>


static bool s_bSaveRam = false;
static std::vector<uint8_t> s_arVRAM;
static uint s_n32kBankMask;

static void aorom_reset();
static void aorom_write(uint address, uint value);
static void aorom_nop();

mapper_t &mapperAOROM()
{
  static mapper_t instance = {
    mapperNROM().read_memory, ppu_read_nop, aorom_write, aorom_nop, aorom_nop, aorom_reset, mnes::mappers::AOROM, false
  };

  return instance;
}


static void aorom_write(uint address, uint value)
{
  static auto &tables = Tables();
  static auto ntram = NTRam();
  static auto &romData = RomData();

  auto rawData = RawData();

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

static void aorom_nop() {}

static void aorom_reset()
{
  static auto &pputable = PPUTable();
  static auto &romData = RomData();
  auto &format = nes_format();
  auto rawData = RawData();

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
