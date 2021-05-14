#include "mapper.h"
#include <cstring>
#include "memory.h"
#include "ppu_memory.h"
#include "ppu_registers.h"
#include "cpu_registers.h"
#include "log.h"

extern uint nrom_read(uint);

static bool		s_bSaveRam;
static uint		s_nLatchSelector0;
static uint		s_nLatchSelector1;
static uint		s_nLatchReg0;
static uint		s_nLatchReg1;
static uint		s_nLatchReg2;
static uint		s_nLatchReg3;
static uchar*		s_pVROM;
static vuchar		s_arVRAM;
static uint		s_n4KbVRomMask;
static uint		s_n16KbPRomMask;

void mmc4_write(uint address, uint value);
void mmc4_reset();
void mmc4_sync();
uint mmc4_ppu_read(uint address);
void mmc4_nop();

SETUP_MAPPER(MMC4, nrom_read, mmc4_write, mmc4_nop, mmc4_nop, mmc4_reset, mmc4_ppu_read)

void mmc4_write(uint address, uint value)
{
	//fuck sram
	if (address < 0xA000) {
		return;
	}

	if (address < 0xB000) {
		uint nBank = (value & s_n16KbPRomMask) * 0x4000;
		for (uint n = 0; n < 4; n++) {
			g_ROM[n] = &g_arRawData[nBank + (0x1000 * n)];
		}
	} else if (address < 0xC000) {
		s_nLatchReg0 = (value & s_n4KbVRomMask);
		mmc4_sync();
	} else if (address < 0xD000) {
		s_nLatchReg1 = (value & s_n4KbVRomMask);
		mmc4_sync();
	} else if (address < 0xE000) {
		s_nLatchReg2 = (value & s_n4KbVRomMask);
		mmc4_sync();
	} else if (address < 0xF000) {
		s_nLatchReg3 = (value & s_n4KbVRomMask);
		mmc4_sync();
	} else if (address < 0x10000) {
		if ((value & 1) == 0) {
			SetVerticalMirror();
		} else {
			SetHorizontalMirror();
		}
	}
}

void mmc4_reset()
{
	s_nLatchSelector0 = 0xFE;
	s_nLatchSelector1 = 0xFE;

	s_n16KbPRomMask = (g_ines_format.prg_rom_count) - 1;

	g_ROM[0] = &g_arRawData[0x0000];
	g_ROM[1] = &g_arRawData[0x1000];
	g_ROM[2] = &g_arRawData[0x2000];
	g_ROM[3] = &g_arRawData[0x3000];

	uint nLastBank = (g_ines_format.prg_rom_count - 1) * 0x4000;

	g_ROM[4] = &g_arRawData[nLastBank + 0x0000];
	g_ROM[5] = &g_arRawData[nLastBank + 0x1000];
	g_ROM[6] = &g_arRawData[nLastBank + 0x2000];
	g_ROM[7] = &g_arRawData[nLastBank + 0x3000];

	nLastBank += 0x4000;

	if (g_ines_format.chr_rom_count > 0) {
		s_n4KbVRomMask = (g_ines_format.chr_rom_count * 2) - 1;
		s_pVROM = &g_arRawData[nLastBank];
	} else {
		s_n4KbVRomMask = 15;
		s_arVRAM.resize(0x2000);
		s_pVROM = &s_arVRAM[0];
	}

	if ((g_ines_format.rom_control_1 & 8) == 8) {
		//dont do this.
		//SetFourScreenMirror();
	} else if ((g_ines_format.rom_control_1 & 1) == 1) {
		SetVerticalMirror();
	} else {
		SetHorizontalMirror();
	}
}

void mmc4_sync()
{
	if (s_nLatchSelector0 == 0xFE) {
		uint nBank = (s_nLatchReg0 & s_n4KbVRomMask) * 0x1000;
		for (uint n = 0; n < 4; n++) {
			g_PPUTable[n] = &s_pVROM[(0x400 * n) + nBank];
		}
	} else if (s_nLatchSelector0 == 0xFD) {
		uint nBank = (s_nLatchReg1 & s_n4KbVRomMask) * 0x1000;
		for (uint n = 0; n < 4; n++) {
			g_PPUTable[n] = &s_pVROM[(0x400 * n) + nBank];
		}
	}

	if (s_nLatchSelector1 == 0xFE) {
		uint nBank = (s_nLatchReg2 & s_n4KbVRomMask) * 0x1000;
		for (uint n = 0; n < 4; n++) {
			g_PPUTable[n + 4] = &s_pVROM[(0x400 * n) + nBank];
		}
	} else if (s_nLatchSelector1 == 0xFD) {
		uint nBank = (s_nLatchReg3 & s_n4KbVRomMask) * 0x1000;
		for (uint n = 0; n < 4; n++) {
			g_PPUTable[n + 4] = &s_pVROM[(0x400 * n) + nBank];
		}
	}
}

uint mmc4_ppu_read(uint address)
{
	if (address >= 0xFD0 && address <= 0xFDF) {
		s_nLatchSelector0 = 0xFD;
		mmc4_sync();
	} else if (address >= 0xFE0 && address <= 0xFEF) {
		s_nLatchSelector0 = 0xFE;
		mmc4_sync();
	} if (address >= 0x1FD0 && address <= 0x1FDF) {
		s_nLatchSelector1 = 0xFD;
		mmc4_sync();
	} else if (address >= 0x1FE0 && address <= 0x1FEF) {
		s_nLatchSelector1 = 0xFE;
		mmc4_sync();
	}
	return address;//open bus
}

void mmc4_nop()
{

}