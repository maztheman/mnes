#include "nrom.h"
#include "mapper.h"
#include <cstring>


#include <ppu/ppu_memory.h>
#include <ppu/ppu_registers.h>
#include <cpu/cpu_registers.h>
#include <cpu/memory.h>
#include <common/Log.h>

//static bool		s_bSaveRam;
static uint		s_nLatchSelector0;
static uint		s_nLatchSelector1;
static uint		s_nLatchReg0;
static uint		s_nLatchReg1;
static uint		s_nLatchReg2;
static uint		s_nLatchReg3;
static uint8_t*	s_pVROM;
static vuchar	s_arVRAM;
static uint		s_n4KbVRomMask;
static uint		s_n16KbPRomMask;

static void mmc4_write(uint address, uint value);
static void mmc4_reset();
static void mmc4_sync();
static uint mmc4_ppu_read(uint address);
static void mmc4_nop();

mapper_t& mapperMMC4()
{
	static mapper_t instance =
	{
		mapperNROM().read_memory, mmc4_ppu_read, mmc4_write, mmc4_nop, mmc4_nop, mmc4_reset, mnes::mappers::MMC4, false
	};
	return instance;
}

static void mmc4_write(uint address, uint value)
{
	static auto& romData = RomData();
	auto rawData = RawData();

	//fuck sram
	if (address < 0xA000) {
		return;
	}

	if (address < 0xB000) {
		uint nBank = (value & s_n16KbPRomMask) * 0x4000;
		for (uint n = 0; n < 4; n++) {
			romData[n] = rawData.subspan(nBank + (0x1000 * n)).data();
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

static void mmc4_reset()
{
	static auto& romData = RomData();
	auto& format = nes_format();
	auto rawData = RawData();

	s_nLatchSelector0 = 0xFE;
	s_nLatchSelector1 = 0xFE;

	s_n16KbPRomMask = (format.prg_rom_count) - 1U;

	romData[0] = rawData.subspan(0x0000).data();
	romData[1] = rawData.subspan(0x1000).data();
	romData[2] = rawData.subspan(0x2000).data();
	romData[3] = rawData.subspan(0x3000).data();

	uint nLastBank = (format.prg_rom_count - 1U) * 0x4000U;

	romData[4] = rawData.subspan(nLastBank + 0x0000).data();
	romData[5] = rawData.subspan(nLastBank + 0x1000).data();
	romData[6] = rawData.subspan(nLastBank + 0x2000).data();
	romData[7] = rawData.subspan(nLastBank + 0x3000).data();

	nLastBank += 0x4000;

	if (format.chr_rom_count > 0) {
		s_n4KbVRomMask = (format.chr_rom_count * 2U) - 1U;
		s_pVROM = rawData.subspan(nLastBank).data();
	} else {
		s_n4KbVRomMask = 15;
		s_arVRAM.resize(0x2000);
		s_pVROM = &s_arVRAM[0];
	}

	if ((format.rom_control_1 & 8) == 8) {
		//dont do this.
		//SetFourScreenMirror();
	} else if ((format.rom_control_1 & 1) == 1) {
		SetVerticalMirror();
	} else {
		SetHorizontalMirror();
	}
}

static void mmc4_sync()
{
	auto& ppuTable = PPUTable();
	if (s_nLatchSelector0 == 0xFE) {
		uint nBank = (s_nLatchReg0 & s_n4KbVRomMask) * 0x1000;
		for (uint n = 0; n < 4; n++) {
			ppuTable[n] = &s_pVROM[(0x400 * n) + nBank];
		}
	} else if (s_nLatchSelector0 == 0xFD) {
		uint nBank = (s_nLatchReg1 & s_n4KbVRomMask) * 0x1000;
		for (uint n = 0; n < 4; n++) {
			ppuTable[n] = &s_pVROM[(0x400 * n) + nBank];
		}
	}

	if (s_nLatchSelector1 == 0xFE) {
		uint nBank = (s_nLatchReg2 & s_n4KbVRomMask) * 0x1000;
		for (uint n = 0; n < 4; n++) {
			ppuTable[n + 4] = &s_pVROM[(0x400 * n) + nBank];
		}
	} else if (s_nLatchSelector1 == 0xFD) {
		uint nBank = (s_nLatchReg3 & s_n4KbVRomMask) * 0x1000;
		for (uint n = 0; n < 4; n++) {
			ppuTable[n + 4] = &s_pVROM[(0x400 * n) + nBank];
		}
	}
}

static uint mmc4_ppu_read(uint address)
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

static void mmc4_nop()
{

}