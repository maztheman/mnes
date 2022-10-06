#include "mapper.h"
#include "memory.h"

#include <ppu/ppu_memory.h>
#include <ppu/ppu_registers.h>
#include <cpu/cpu_registers.h>
#include <common/Log.h>

extern uint nrom_read(uint);

static uint					s_nLatchSelector;
static uint					s_nLatchReg0;
static uint					s_nLatchReg1;
static uint					s_n8KbPRomMask;
static uint					s_n4KbVRomMask;
static uchar*				s_pVROM;
static std::vector<uchar>	s_arVRAM;

void mmc2_sync();

void mmc2_write(uint address, uint value)
{
	if (address < 0xA000) {
		return;
	}

	if (address < 0xB000) {
		uint nBank = (value & s_n8KbPRomMask) * 0x2000;
		g_ROM[0] = &g_arRawData[nBank];
		g_ROM[1] = &g_arRawData[nBank + 0x1000];
	} else if (address < 0xD000) {
		uint nBank = (value & s_n4KbVRomMask) * 0x1000;
		for (uint n = 0; n < 4; n++) {
			g_PPUTable[n] = &s_pVROM[(0x400 * n) + nBank];
		}
	} else if (address < 0xE000) {
		s_nLatchReg0 = (value & s_n4KbVRomMask);
		mmc2_sync();
	} else if (address < 0xF000) {
		s_nLatchReg1 = (value & s_n4KbVRomMask);
		mmc2_sync();
	} else if (address < 0x10000) {
		if ((value & 1) == 0) {
			SetVerticalMirror();
		} else {
			SetHorizontalMirror();
		}
	}
}

void mmc2_sync()
{
	if (s_nLatchSelector == 0xFE) {
		uint nBank = (s_nLatchReg1 & s_n4KbVRomMask) * 0x1000;
		for (uint n = 0; n < 4; n++) {
			g_PPUTable[n + 4] = &s_pVROM[(0x400 * n) + nBank];
		}
	} else if (s_nLatchSelector == 0xFD) {
		uint nBank = (s_nLatchReg0 & s_n4KbVRomMask) * 0x1000;
		for (uint n = 0; n < 4; n++) {
			g_PPUTable[n + 4] = &s_pVROM[(0x400 * n) + nBank];
		}
	}
}

void mmc2_reset()
{
	s_nLatchSelector = 0xFE;
	s_pVROM = nullptr;

	s_n8KbPRomMask = (g_ines_format.prg_rom_count * 2) - 1;

	for (uint n = 0; n < 2; n++) {
		g_ROM[n] = &g_arRawData[(0x1000) * n];
	}

	uint nLastBank = ((g_ines_format.prg_rom_count * 2) - 3) * 0x2000;
	uint tmp = 0x0000;
	for (uint n = 2; n < 8; n++, tmp += 0x1000) {
		g_ROM[n] = &g_arRawData[nLastBank + tmp];
	}

	nLastBank += 0x6000;

	if (g_ines_format.chr_rom_count > 0) {
		s_n4KbVRomMask = (g_ines_format.chr_rom_count * 2) - 1;
		s_pVROM = &g_arRawData[nLastBank];
	} else {
		s_n4KbVRomMask = 15;
		s_arVRAM.resize(0x2000);
		s_pVROM = &s_arVRAM[0];
	}

	if ((g_ines_format.rom_control_1 & 8) == 8) {
		//SetFourScreenMirror();
	} else if ((g_ines_format.rom_control_1 & 1) == 1) {
		SetVerticalMirror();
	} else {
		SetHorizontalMirror();
	}
}

uint mmc2_ppu_read(uint address)
{
	uint test = address & 0xFFF;
	if (test >= 0xFD0 && test <= 0xFDF) {
		s_nLatchSelector = 0xFD;
		mmc2_sync();
	} else if (test >= 0xFE0 && test <= 0xFEF) {
		s_nLatchSelector = 0xFE;
		mmc2_sync();
	}
	return address;//open bus
}

void mmc2_nop()
{

}

SETUP_MAPPER(MMC2, nrom_read, mmc2_write, mmc2_nop, mmc2_nop, mmc2_reset, mmc2_ppu_read)