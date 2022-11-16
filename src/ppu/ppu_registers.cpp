#include "ppu_registers.h"

ppu_registers& PPURegs()
{
	static ppu_registers instance = {241, (uint)-5};
	return instance;
}

uint ppu_scanline() 
{
	return PPURegs().scanline;
}