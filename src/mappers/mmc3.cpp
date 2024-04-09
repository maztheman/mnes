#include "mmc3.h"
#include "nrom.h"

#include "mapper.h"

#include <cstring>

#include <cpu/cpu_registers.h>
#include <cpu/memory.h>
#include <cpu/memory_registers.h>

#include <ppu/ppu_memory.h>
#include <ppu/ppu_registers.h>

#include <common/Log.h>

using namespace mnes;
using namespace mnes::mappers;
using mnes::memory::SRam;

namespace {
bool s_bSaveRam;
bool s_bDisableIRQ;
bool s_bChipEnable;
bool s_bForceReload;

uint32_t s_r8000;
uint32_t s_r8001;
uint32_t s_rA000;
uint32_t s_rA001;
/// <summary>
/// IRQ Counter Register
/// </summary>
uint32_t s_rC000;
/// <summary>
/// IRQ Latch Register
/// </summary>
uint32_t s_rC001;
uint32_t s_rE000;
uint32_t s_rE001;

uint32_t s_nPRGCount;
uint32_t s_nLastBank;
uint32_t s_nScanlineCount;
uint8_t *s_pVROM;
uint32_t s_nVromMask;
vuchar s_arNTRAM;
auto &romData = current_rom_data();
auto cvram = cart_vram();
auto &format = current_nes_format();
auto &ptable = ppu::memory::pattern_table();
auto &ntable = ppu::memory::name_table();
auto ntRam = ppu::memory::NTRam();


namespace mmc3 {
  void write(uint32_t address, uint32_t value);
  void reset();
  void ppu_do_cycle();
  void nop();
}

mapper_t this_mapper = { mapperNROM().read_memory,
    ppu_read_nop,
    mmc3::write,
    mmc3::nop,
    mmc3::ppu_do_cycle,
    mmc3::reset,
    MMC3,
    false };
}

mnes::mappers::mapper_t &mnes::mappers::mapperMMC3()
{
  return this_mapper;
}

namespace {
void mmc3::write(uint32_t address, uint32_t value)
{
  auto rawData = current_raw_data();

  // could handle writes to save ram here to keep the save ram local to mapper butt fuck it for now.

  // before sram man
  if (address < 0x6000) { return; }

  if (address < 0x8000) {
    if (s_bSaveRam) { SRam()[address & 0x1FFF] = value & 0xFF; }
    return;
  }

  // m_Log.AddLine("%04X #$%02X SL:%03ld MMC SL:%03ld", address, value, ppu::scanline(), m_nScanlineCount);

  if (address < 0xA000) {
    bool bReg = (address & 1) == 0;
    if (bReg) {
      s_r8000 = value;
    } else {
      s_r8001 = value;
      uint32_t nCommand = s_r8000 & 7;

      switch (nCommand) {
      case 0:
      case 1: {
        uint32_t nRomBank = ((s_r8001 & s_nVromMask) * 0x400);
        uint32_t nAddress = nCommand * 0x800;
        if ((s_r8000 & 0x80) == 0x80) { nAddress ^= 0x1000; }
        // convert to bank
        nAddress >>= 0xA;
        ptable[nAddress] = &s_pVROM[nRomBank];
        ptable[nAddress + 1] = &s_pVROM[nRomBank + 0x400];
      } break;
      case 2:
      case 3:
      case 4:
      case 5: {
        uint32_t nRomBank = ((s_r8001 & s_nVromMask) * 0x400);
        uint32_t nAddress = 0x1000 + ((nCommand - 2) * 0x400);
        if ((s_r8000 & 0x80) == 0x80) { nAddress ^= 0x1000; }

        // convert to bank
        nAddress >>= 0xA;
        ptable[nAddress] = &s_pVROM[nRomBank];
      } break;
      case 6: {
        size_t nBank = 0;
        uint32_t nHardWire = (s_nPRGCount - 1) * 0x4000;
        if ((s_r8000 & 0x40) == 0x40) {
          romData[0] = rawData.subspan(nHardWire).data();// 0x8000-9FFF is hardwires to 2nd last bank
          romData[1] = rawData.subspan(nHardWire + 0x1000).data();
          nBank = 4;// 0xC000
        } else {
          romData[4] = rawData.subspan(nHardWire).data();// 0xC000-DFFF is hardwires to 2nd last bank
          romData[5] = rawData.subspan(nHardWire + 0x1000).data();
          nBank = 0;// 0x8000
        }

        size_t nRomBank = 0x2000 * (s_r8001 & ((s_nPRGCount * 2) - 1));
        romData[nBank] = rawData.subspan(nRomBank).data();
        romData[nBank + 1] = rawData.subspan(nRomBank + 0x1000UL).data();
      } break;
      case 7: {
        uint32_t nRomBank = 0x2000 * (s_r8001 & ((s_nPRGCount * 2) - 1));
        romData[2] = rawData.subspan(nRomBank).data();
        romData[3] = rawData.subspan(nRomBank + 0x1000).data();
      } break;
      }
    }
  } else if (address < 0xC000) {
    bool bReg = (address & 1) == 0;// even
    if (bReg) {
      s_rA000 = value;
    } else {
      s_rA001 = value;
      if ((s_rA000 & 1) == 0) {
        SetVerticalMirror();
      } else {
        SetHorizontalMirror();
      }
      s_bChipEnable = (s_rA001 & 0x80) == 0x80;
      this_mapper.m_bSaveRam = s_bSaveRam = (s_rA001 & 0x40) == 0x0;
    }
  } else if (address < 0xE000) {
    bool bReg = (address & 1) == 0;
    if (bReg) {
      s_rC000 = value;
    } else {
      s_rC001 = value;
      s_bForceReload = true;
    }
  } else if (address < 0x10000) {
    bool bReg = (address & 1) == 0;
    if (bReg) {
      // this value doesnt seem to be used??
      s_rE000 = value;
      // Any value written here 1) disables irq
      s_bDisableIRQ = true;
      // is this how we ackknowledge interrupts?
      cpu::clear_mapper1_irq();
    } else {
      // this value doesnt seem to be used??
      s_rE001 = value;
      // any value here enables irq's
      s_bDisableIRQ = false;
    }
  }
  // m_Log.AddLine(" r8000: %02X r8001: %02X rA000: %02X rA001: %02X rC000: %02X rC001: %02X rE000: %02X rE001: %02X\n",
  //	m_r8000, m_r8001, m_rA000, m_rA001, m_rC000, m_rC001, m_rE000, m_rE001);
}

void SetFourScreenMirror()
{
  s_arNTRAM.resize(0x800);
  ntable[0] = ntRam.data();
  ntable[1] = ntRam.subspan(0x400).data();
  ntable[2] = &s_arNTRAM[0x000];
  ntable[3] = &s_arNTRAM[0x400];
}


void mmc3::reset()
{
  auto rawData = current_raw_data();
  // mmc3_log.Start("logs/mmc3.log");
  this_mapper.m_bSaveRam = s_bSaveRam = true;
  s_bDisableIRQ = false;
  s_bChipEnable = true;
  s_bForceReload = false;
  s_r8000 = 0;
  s_r8001 = 0;
  s_rA000 = 0;
  s_rA001 = 0;
  s_rC000 = 0;
  s_rE000 = 0;
  s_rE001 = 0;
  s_nScanlineCount = 241;

  cpu::clear_mapper1_irq();

  romData[0] = rawData.subspan(0x0000).data();
  romData[1] = rawData.subspan(0x1000).data();
  romData[2] = rawData.subspan(0x2000).data();
  romData[3] = rawData.subspan(0x3000).data();

  s_nPRGCount = format.prg_rom_count;

  s_nLastBank = (s_nPRGCount - 1) * 0x4000;

  romData[4] = rawData.subspan(s_nLastBank + 0x0000).data();
  romData[5] = rawData.subspan(s_nLastBank + 0x1000).data();
  romData[6] = rawData.subspan(s_nLastBank + 0x2000).data();
  romData[7] = rawData.subspan(s_nLastBank + 0x3000).data();

  if (format.chr_rom_count > 0) {
    s_nVromMask = (8 * format.chr_rom_count) - 1U;
    s_nLastBank += 0x4000;
    s_pVROM = rawData.subspan(s_nLastBank).data();
    for (uint32_t n = 0; n < 8; n++) {
      // first 2k is swapped in at reset
      ptable[n] = &s_pVROM[(0x400 * n)];
    }
  } else {
    s_nVromMask = 7;
    s_pVROM = cvram.data();
    for (uint32_t n = 0; n < 8; n++) { ptable[n] = &s_pVROM[(0x400 * n)]; }
  }

  if ((format.rom_control_1 & 8) == 8) {
    SetFourScreenMirror();
  } else if ((format.rom_control_1 & 1) == 1) {
    SetVerticalMirror();
  } else {
    SetHorizontalMirror();
  }
}

bool is_a12_rising(const uint32_t s_ui_ppu_addr_last, const uint32_t ui_ppu_addr)
{
  auto last_is_zero = (s_ui_ppu_addr_last & 0x1000) == 0;
  auto now_is_1 = (ui_ppu_addr & 0x1000) == 0x1000;
  return last_is_zero && now_is_1;
}


void mmc3::ppu_do_cycle()
{
  static uint32_t s_ui_ppu_addr_last = 0;

  auto ui_ppu_addr = memory::ppu_addr_bus();

  if (ui_ppu_addr >= 0x3F00 && ui_ppu_addr <= 0x3FFF) { ui_ppu_addr = s_ui_ppu_addr_last; }

  if (is_a12_rising(s_ui_ppu_addr_last, ui_ppu_addr)) {
    // clocked
    if (s_nScanlineCount == 0 || s_bForceReload) {
      s_bForceReload = false;
      s_nScanlineCount = s_rC000;
    } else {
      s_nScanlineCount--;
    }

    if (s_nScanlineCount == 0 && s_bDisableIRQ == false) { cpu::set_mapper1_irq(); }
  }
  s_ui_ppu_addr_last = ui_ppu_addr;
}

void mmc3::nop() {}
}