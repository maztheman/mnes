#pragma once


static constexpr uint32_t NULL_LAST_READ = static_cast<uint32_t>(-5);

struct ppu_registers 
{
	uint32_t scanline;
	uint32_t last_2002_read;
};

ppu_registers& PPURegs();
uint32_t ppu_scanline();
