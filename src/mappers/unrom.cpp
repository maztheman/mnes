#include "unrom.h"

#include "nrom.h"

#include "mapper.h"

#include <ppu/ppu_memory.h>

#include <cstring>
#include <vector>


static uint8_t s_VRAM[0x2000];

static void unrom_write(uint address, uint value);
static void unrom_nop();
static void unrom_reset();

mapper_t& mapperUNROM()
{
	static mapper_t instance = 
	{
		mapperNROM().read_memory, ppu_read_nop, unrom_write, unrom_nop, unrom_nop, unrom_reset, mnes::mappers::UNROM, false
	};

	return instance;
}

static void unrom_write(uint address, uint value)
{
	static auto& romData = RomData();
	auto rawData = RawData();

	if (address < 0x8000) {
		return;
	}

	uint nBankAddress = value * 0x4000;

	romData[0] = rawData.subspan(nBankAddress + 0x0000).data();
	romData[1] = rawData.subspan(nBankAddress + 0x1000).data();
	romData[2] = rawData.subspan(nBankAddress + 0x2000).data();
	romData[3] = rawData.subspan(nBankAddress + 0x3000).data();
}

static void unrom_nop() 
{

}

static void unrom_reset()
{
	static auto& romData = RomData();
	auto& format = nes_format();
	auto rawData = RawData();

	romData[0] = rawData.subspan(0x0000).data();
	romData[1] = rawData.subspan(0x1000).data();
	romData[2] = rawData.subspan(0x2000).data();
	romData[3] = rawData.subspan(0x3000).data();

	uint nLastBank = (format.prg_rom_count - 1U) * 0x4000U;

	romData[4] = rawData.subspan(nLastBank + 0x0000).data();
	romData[5] = rawData.subspan(nLastBank + 0x1000).data();
	romData[6] = rawData.subspan(nLastBank + 0x2000).data();
	romData[7] = rawData.subspan(nLastBank + 0x3000).data();

	memset(&s_VRAM[0], 0, 0x2000);

	static auto& ppuTable = PPUTable();

	for (uint n = 0; n < 8; n++) {
		ppuTable[n] = &s_VRAM[(0x400 * n)];
	}

	if ((format.rom_control_1 & 1) == 1) {
		SetVerticalMirror();
	} else {
		SetHorizontalMirror();
	}
}
