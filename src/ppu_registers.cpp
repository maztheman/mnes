#include "ppu_registers.h"

ppu_registers g_PPURegisters = {241, (uint)-5};

uint ppu_scanline() {
	return g_PPURegisters.scanline;
}