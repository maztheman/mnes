#include "ppu_registers.h"

ppu_registers& PPURegs()
{
	static ppu_registers instance = {241, NULL_LAST_READ};
	return instance;
}

uint ppu_scanline() 
{
	return PPURegs().scanline;
}