#include "cnrom.h"

#include "nrom.h"

#include "mapper.h"

#include <ppu/ppu_memory.h>


static size_t s_nOffset;

static void cnrom_write(uint address, uint value);
static void cnrom_reset();
static void cnrom_nop();

mapper_t &mapperCNROM()
{
  static mapper_t instance = {
    mapperNROM().read_memory, ppu_read_nop, cnrom_write, cnrom_nop, cnrom_nop, cnrom_reset, mnes::mappers::CNROM, false
  };
  return instance;
}

static void cnrom_write(uint address, uint value)
{
  if (address < 0x8000) { return; }

  size_t nBankAddress = (value * 0x2000) + s_nOffset;

  static auto &pputable = PPUTable();

  auto rawData = RawData();

  // ppu pages are 1k or 0x400
  for (size_t n = 0; n < 8; n++) { pputable[n] = rawData.subspan(nBankAddress + (0x400 * n)).data(); }
}

static void cnrom_reset()
{
  static auto &romData = RomData();

  auto &format = nes_format();
  auto rawData = RawData();

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
  for (uint n = 0; n < 8; n++) { PPUTable()[n] = rawData.subspan(s_nOffset + (0x400 * n)).data(); }

  if ((format.rom_control_1 & 1) == 1) {
    SetVerticalMirror();
  } else {
    SetHorizontalMirror();
  }
}

static void cnrom_nop() {}
