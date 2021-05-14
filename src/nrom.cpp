#include "ppu_memory.h"
#include "mapper.h"
#include "memory.h"

extern ines_format					g_ines_format;
extern std::vector<unsigned char>	g_arRawData;

static std::vector<unsigned char> s_arVRAM;

uint nrom_read(uint address)
{
	if (address >= 0x6000 && address < 0x8000) {
		extern mapper_t* g_mapper;
		if (g_mapper->m_bSaveRam) {
			return g_SRAM[address & 0x1FFF];
		}
	}

	if (address >= 0x8000 && address <= 0xFFFF) {
		uint bank = (address >> 0xC) - 8;
		return g_ROM[bank][address & 0xFFF];
	}

	return 0;
}

void nrom_write(uint address, uint value)
{

}

void nrom_nop()
{

}

void nrom_reset()
{
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

	if (g_ines_format.chr_rom_count == 0) {
		s_arVRAM.resize(0x2000, 0);
		//ppu pages are 1k or 0x400
		for (uint n = 0; n < 8; n++) {
			g_PPUTable[n] = &s_arVRAM[(0x400 * n)];
		}
	} else {
		//ppu pages are 1k or 0x400
		for (uint n = 0; n < 8; n++) {
			g_PPUTable[n] = &g_arRawData[nOffset + (0x400 * n)];
		}
	}

	if ((g_ines_format.rom_control_1 & 1) == 1) {
		SetVerticalMirror();
	} else {
		SetHorizontalMirror();
	}
	
}



SETUP_MAPPER(NROM, nrom_read, nrom_write, nrom_nop, nrom_nop, nrom_reset, ppu_read_nop)
