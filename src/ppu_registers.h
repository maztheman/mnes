#ifndef __PPU_REGISTERS_H__
#define __PPU_REGISTERS_H__

typedef unsigned int uint;

struct ppu_registers {
	uint scanline;
	uint last_2002_read;
};

extern ppu_registers g_PPURegisters;

uint ppu_scanline();


#endif