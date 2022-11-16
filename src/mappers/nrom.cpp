#include "nrom.h"

#include "mapper.h"

#include <ppu/ppu_memory.h>
#include <cpu/memory.h>


extern ines_format					g_ines_format;
extern std::vector<unsigned char>	g_arRawData;

static std::vector<unsigned char> s_arVRAM;


static uint nrom_read(uint address);
static void nrom_write(uint, uint);
static void nrom_nop();
static void nrom_reset();

mapper_t& mapperNROM()
{
	static mapper_t instance = 
	{
		nrom_read, ppu_read_nop, nrom_write, nrom_nop, nrom_nop, nrom_reset, mnes::mappers::NROM, false
	};
	return instance;
}


static uint nrom_read(uint address)
{
	if (address >= 0x6000 && address < 0x8000) {
		if (current_mapper()->m_bSaveRam) {
			return g_SRAM[address & 0x1FFF];
		}
	}

	if (address >= 0x8000 && address <= 0xFFFF) {
		uint bank = (address >> 0xC) - 8;
		return g_ROM[bank][address & 0xFFF];
	}

	return 0;
}

static void nrom_write(uint, uint)
{

}

static void nrom_nop()
{

}

static void nrom_reset()
{
	printf("Mapper[nrom]:reset()\n");
	uint nOffset = 0x4000;
	if (g_ines_format.prg_rom_count == 1) {
		//banks are 0x1000 in size
		g_ROM[4] = g_ROM[0] = &g_arRawData[0x0000];
		g_ROM[5] = g_ROM[1] = &g_arRawData[0x1000];
		g_ROM[6] = g_ROM[2] = &g_arRawData[0x2000];
		g_ROM[7] = g_ROM[3] = &g_arRawData[0x3000];
	} else {//2 
		nOffset = 0x8000;
		g_ROM[0] = &g_arRawData[0x0000];
		g_ROM[1] = &g_arRawData[0x1000];
		g_ROM[2] = &g_arRawData[0x2000];
		g_ROM[3] = &g_arRawData[0x3000];
		g_ROM[4] = &g_arRawData[0x4000];
		g_ROM[5] = &g_arRawData[0x5000];
		g_ROM[6] = &g_arRawData[0x6000];
		g_ROM[7] = &g_arRawData[0x7000];
	}

	auto& ppuTable = PPUTable();

	if (g_ines_format.chr_rom_count == 0) {
		s_arVRAM.resize(0x2000, 0);
		//ppu pages are 1k or 0x400
		for (uint n = 0; n < 8; n++) {
			ppuTable[n] = &s_arVRAM[(0x400 * n)];
		}
	} else {
		//ppu pages are 1k or 0x400
		for (uint n = 0; n < 8; n++) {
			ppuTable[n] = &g_arRawData[nOffset + (0x400 * n)];
		}
	}

	if ((g_ines_format.rom_control_1 & 1) == 1) {
		SetVerticalMirror();
	} else {
		SetHorizontalMirror();
	}
}
