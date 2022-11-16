#include "unrom.h"

#include "nrom.h"

#include "mapper.h"

#include <ppu/ppu_memory.h>

#include <cstring>
#include <vector>


extern std::vector<unsigned char> g_arRawData;
extern ines_format g_ines_format;

static uchar s_VRAM[0x2000];

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
	if (address < 0x8000) {
		return;
	}

	uint nBankAddress = value * 0x4000;

	g_ROM[0] = &g_arRawData[nBankAddress + 0x0000];
	g_ROM[1] = &g_arRawData[nBankAddress + 0x1000];
	g_ROM[2] = &g_arRawData[nBankAddress + 0x2000];
	g_ROM[3] = &g_arRawData[nBankAddress + 0x3000];
}

static void unrom_nop() 
{

}

static void unrom_reset()
{
	g_ROM[0] = &g_arRawData[0x0000];
	g_ROM[1] = &g_arRawData[0x1000];
	g_ROM[2] = &g_arRawData[0x2000];
	g_ROM[3] = &g_arRawData[0x3000];


	uint nLastBank = (g_ines_format.prg_rom_count - 1) * 0x4000;

	g_ROM[4] = &g_arRawData[nLastBank + 0x0000];
	g_ROM[5] = &g_arRawData[nLastBank + 0x1000];
	g_ROM[6] = &g_arRawData[nLastBank + 0x2000];
	g_ROM[7] = &g_arRawData[nLastBank + 0x3000];

	memset(&s_VRAM[0], 0, 0x2000);

	static auto& ppuTable = PPUTable();

	for (uint n = 0; n < 8; n++) {
		ppuTable[n] = &s_VRAM[(0x400 * n)];
	}

	if ((g_ines_format.rom_control_1 & 1) == 1) {
		SetVerticalMirror();
	} else {
		SetHorizontalMirror();
	}
}
