#include "mapper.h"
#include "ppu_memory.h"

extern uint nrom_read(uint);
static size_t s_nOffset;

void cnrom_write(uint address, uint value)
{
	if (address < 0x8000) {
		return;
	}

	uint nBankAddress = (value * 0x2000) + s_nOffset;

	//ppu pages are 1k or 0x400
	for (uint n = 0; n < 8; n++) {
		g_PPUTable[n] = &g_arRawData[nBankAddress + (0x400 * n)];
	}
}

void cnrom_reset()
{
	s_nOffset = 0x4000;
	if (g_ines_format.prg_rom_count == 1) {
		//banks are 0x1000 in size
		g_ROM[4] = g_ROM[0] = &g_arRawData[0x0000];
		g_ROM[5] = g_ROM[1] = &g_arRawData[0x1000];
		g_ROM[6] = g_ROM[2] = &g_arRawData[0x2000];
		g_ROM[7] = g_ROM[3] = &g_arRawData[0x3000];
	} else {
		//2
		s_nOffset = 0x8000;
		g_ROM[0] = &g_arRawData[0x0000];
		g_ROM[1] = &g_arRawData[0x1000];
		g_ROM[2] = &g_arRawData[0x2000];
		g_ROM[3] = &g_arRawData[0x3000];
		g_ROM[4] = &g_arRawData[0x4000];
		g_ROM[5] = &g_arRawData[0x5000];
		g_ROM[6] = &g_arRawData[0x6000];
		g_ROM[7] = &g_arRawData[0x7000];
	}

	//ppu pages are 1k or 0x400
	for (uint n = 0; n < 8; n++) {
		g_PPUTable[n] = &g_arRawData[s_nOffset + (0x400 * n)];
	}

	if ((g_ines_format.rom_control_1 & 1) == 1) {
		SetVerticalMirror();
	} else {
		SetHorizontalMirror();
	}
}

void cnrom_nop()
{

}

SETUP_MAPPER(CNROM, nrom_read, cnrom_write, cnrom_nop, cnrom_nop, cnrom_reset, ppu_read_nop)