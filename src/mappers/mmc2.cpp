#include "mmc2.h"
#include "nrom.h"


#include "mapper.h"
#include "memory.h"

#include <common/Log.h>
#include <cpu/cpu_registers.h>
#include <ppu/ppu_memory.h>
#include <ppu/ppu_registers.h>

using namespace mnes;
using namespace mnes::mappers;

namespace {
uint32_t s_nLatchSelector;
uint32_t s_nLatchReg0;
uint32_t s_nLatchReg1;
uint32_t s_n8KbPRomMask;
uint32_t s_n4KbVRomMask;
uint8_t *s_pVROM;
auto &romData = current_rom_data();
auto &format = current_nes_format();
auto cvram = cart_vram();
auto &ptable = ppu::memory::pattern_table();

namespace mmc2 {
  void sync();
  void write(uint32_t address, uint32_t value);
  void reset();
  uint32_t ppu_read(uint32_t address);
  void nop();
}

mapper_t this_mapper = {
    mapperNROM().read_memory, mmc2::ppu_read, mmc2::write, mmc2::nop, mmc2::nop, mmc2::reset, MMC2, false
};
}

mnes::mappers::mapper_t &mnes::mappers::mapperMMC2()
{
  return this_mapper;
}

namespace {
void mmc2::write(uint32_t address, uint32_t value)
{
  auto rawData = current_raw_data();

  if (address < 0xA000) { return; }

  if (address < 0xB000) {
    uint32_t nBank = (value & s_n8KbPRomMask) * 0x2000;
    romData[0] = rawData.subspan(nBank).data();
    romData[1] = rawData.subspan(nBank + 0x1000).data();
  } else if (address < 0xD000) {
    uint32_t nBank = (value & s_n4KbVRomMask) * 0x1000;
    for (uint32_t n = 0; n < 4; n++) { ptable[n] = &s_pVROM[(0x400 * n) + nBank]; }
  } else if (address < 0xE000) {
    s_nLatchReg0 = (value & s_n4KbVRomMask);
    mmc2::sync();
  } else if (address < 0xF000) {
    s_nLatchReg1 = (value & s_n4KbVRomMask);
    mmc2::sync();
  } else if (address < 0x10000) {
    if ((value & 1) == 0) {
      SetVerticalMirror();
    } else {
      SetHorizontalMirror();
    }
  }
}

void mmc2::sync()
{
  if (s_nLatchSelector == 0xFE) {
    uint32_t nBank = (s_nLatchReg1 & s_n4KbVRomMask) * 0x1000;
    for (uint32_t n = 0; n < 4; n++) { ptable[n + 4] = &s_pVROM[(0x400 * n) + nBank]; }
  } else if (s_nLatchSelector == 0xFD) {
    uint32_t nBank = (s_nLatchReg0 & s_n4KbVRomMask) * 0x1000;
    for (uint32_t n = 0; n < 4; n++) { ptable[n + 4] = &s_pVROM[(0x400 * n) + nBank]; }
  }
}

void mmc2::reset()
{
  auto rawData = current_raw_data();

  s_nLatchSelector = 0xFE;
  s_pVROM = nullptr;

  s_n8KbPRomMask = (format.prg_rom_count * 2U) - 1U;

  for (uint32_t n = 0; n < 2; n++) { romData[n] = rawData.subspan((0x1000) * n).data(); }

  uint32_t nLastBank = ((format.prg_rom_count * 2U) - 3U) * 0x2000U;
  uint32_t tmp = 0x0000;
  for (uint32_t n = 2; n < 8; n++, tmp += 0x1000) { romData[n] = rawData.subspan(nLastBank + tmp).data(); }

  nLastBank += 0x6000;

  if (format.chr_rom_count > 0) {
    s_n4KbVRomMask = (format.chr_rom_count * 2U) - 1U;
    s_pVROM = rawData.subspan(nLastBank).data();
  } else {
    s_n4KbVRomMask = 15;
    s_pVROM = cvram.data();
  }

  if ((format.rom_control_1 & 8) == 8) {
    // SetFourScreenMirror();
  } else if ((format.rom_control_1 & 1) == 1) {
    SetVerticalMirror();
  } else {
    SetHorizontalMirror();
  }
}

uint32_t mmc2::ppu_read(uint32_t address)
{
  uint32_t test = address & 0xFFF;
  if (test >= 0xFD0 && test <= 0xFDF) {
    s_nLatchSelector = 0xFD;
    mmc2::sync();
  } else if (test >= 0xFE0 && test <= 0xFEF) {
    s_nLatchSelector = 0xFE;
    mmc2::sync();
  }
  return address;// open bus
}

void mmc2::nop() {}
}