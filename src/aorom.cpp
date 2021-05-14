#include <cstring>
#include "memory.h"
#include "ppu_memory.h"
#include "ppu_registers.h"
#include "cpu_registers.h"
#include "log.h"
#include "mapper.h"


extern ines_format					g_ines_format;
extern vuchar						g_arRawData;
extern uint nrom_read(uint);

static bool					s_bSaveRam = false;
static std::vector<uchar>	s_arVRAM;
static uint					s_n32kBankMask;

void aorom_reset();


void aorom_write(uint address, uint value)
{
	if (address >= 0x6000 && address < 0x8000) {
		if (s_bSaveRam) {
			g_SRAM[address & 0x1FFF] = value & 0xFF;
		}
	}

	if (address < 0x8000) {
		return;
	}

	if ((value & 0x10) == 0x10) {
		g_Tables[0] = g_Tables[1] = g_Tables[2] = g_Tables[3] = &g_NTRam[0x400];
	} else {
		g_Tables[0] = g_Tables[1] = g_Tables[2] = g_Tables[3] = &g_NTRam[0x000];
	}

	uint n32kBank = ((value & 0xF) & s_n32kBankMask) * 0x8000;

	for (uint n = 0; n < 8; n++) {
		g_ROM[n] = &g_arRawData[n32kBank + (0x1000 * n)];
	}
}

void aorom_nop()
{

}


SETUP_MAPPER(AOROM, nrom_read, aorom_write, aorom_nop, aorom_nop, aorom_reset, ppu_read_nop)

void aorom_reset()
{
	for (uint n = 0; n < 8; n++) {
		g_ROM[n] = &g_arRawData[(0x1000 * n)];
	}

	s_arVRAM.resize(0x2000);
	for (uint n = 0; n < 8; n++) {
		g_PPUTable[n] = &s_arVRAM[(0x400 * n)];
	}

	s_n32kBankMask = (g_ines_format.prg_rom_count * 8) - 1;

	if ((g_ines_format.rom_control_1 & 8) == 8) {
		//don't do this
		//		SetFourScreenMirror();
	} else if ((g_ines_format.rom_control_1 & 1) == 1) {
		SetVerticalMirror();
	} else {
		SetHorizontalMirror();
	}

	 s_bSaveRam = maprAOROM.m_bSaveRam = (g_ines_format.rom_control_1 & 2) == 2;
}