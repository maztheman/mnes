
#include "nrom.h"

#include "mapper.h"
#include "memory.h"

#include <ppu/ppu_memory.h>
#include <ppu/ppu_registers.h>
#include <cpu/cpu_registers.h>
#include <common/Log.h>

static uint					s_nLatchSelector;
static uint					s_nLatchReg0;
static uint					s_nLatchReg1;
static uint					s_n8KbPRomMask;
static uint					s_n4KbVRomMask;
static uint8_t*				s_pVROM;
static std::vector<uint8_t>	s_arVRAM;

static void mmc2_sync();
static void mmc2_write(uint address, uint value);
static void mmc2_reset();
static uint mmc2_ppu_read(uint address);
static void mmc2_nop();

mapper_t& mapperMMC2()
{
	static mapper_t instance =
	{
		mapperNROM().read_memory, mmc2_ppu_read, mmc2_write, mmc2_nop, mmc2_nop, mmc2_reset, mnes::mappers::MMC2, false
	};
	return instance;
}

static void mmc2_write(uint address, uint value)
{
	static auto& romData = RomData();
	auto rawData = RawData();

	if (address < 0xA000) 
	{
		return;
	}

	if (address < 0xB000) {
		uint nBank = (value & s_n8KbPRomMask) * 0x2000;
		romData[0] = rawData.subspan(nBank).data();
		romData[1] = rawData.subspan(nBank + 0x1000).data();
	} else if (address < 0xD000) {
		uint nBank = (value & s_n4KbVRomMask) * 0x1000;
		auto& ppuTable = PPUTable();
		for (uint n = 0; n < 4; n++) {
			ppuTable[n] = &s_pVROM[(0x400 * n) + nBank];
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

static void mmc2_sync()
{
	auto& ppuTable = PPUTable();

	if (s_nLatchSelector == 0xFE) {
		uint nBank = (s_nLatchReg1 & s_n4KbVRomMask) * 0x1000;
		for (uint n = 0; n < 4; n++) {
			ppuTable[n + 4] = &s_pVROM[(0x400 * n) + nBank];
		}
	} else if (s_nLatchSelector == 0xFD) {
		uint nBank = (s_nLatchReg0 & s_n4KbVRomMask) * 0x1000;
		for (uint n = 0; n < 4; n++) {
			ppuTable[n + 4] = &s_pVROM[(0x400 * n) + nBank];
		}
	}
}

static void mmc2_reset()
{
	static auto& romData = RomData();
	auto& format = nes_format();
	auto rawData = RawData();

	s_nLatchSelector = 0xFE;
	s_pVROM = nullptr;

	s_n8KbPRomMask = (format.prg_rom_count * 2) - 1;

	for (uint n = 0; n < 2; n++) {
		romData[n] = rawData.subspan((0x1000) * n).data();
	}

	uint nLastBank = ((format.prg_rom_count * 2) - 3) * 0x2000;
	uint tmp = 0x0000;
	for (uint n = 2; n < 8; n++, tmp += 0x1000) {
		romData[n] = rawData.subspan(nLastBank + tmp).data();
	}

	nLastBank += 0x6000;

	if (format.chr_rom_count > 0) {
		s_n4KbVRomMask = (format.chr_rom_count * 2) - 1;
		s_pVROM = rawData.subspan(nLastBank).data();
	} else {
		s_n4KbVRomMask = 15;
		s_arVRAM.resize(0x2000);
		s_pVROM = &s_arVRAM[0];
	}

	if ((format.rom_control_1 & 8) == 8) {
		//SetFourScreenMirror();
	} else if ((format.rom_control_1 & 1) == 1) {
		SetVerticalMirror();
	} else {
		SetHorizontalMirror();
	}
}

static uint mmc2_ppu_read(uint address)
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

static void mmc2_nop()
{

}