#include "ppu_memory.h"

#include <cpu/memory_registers.h>

uchar*	g_PPUTable[8]	= {0};
uchar*	g_Tables[4]		= {0};
//optional vram, this will swap out the memory...maybe can wrap the memory so we dealloc after...
uchar	g_Palette[64]	= {0};
uchar	g_NTRam[0x800]	= {0};

uint ppu_memory_main_read(uint address)
{
	//g_MemoryRegisters.ppu_addr_bus = address;

	address &= 0x3FFF;

	current_mapper()->read_ppu_memory(address);//allow mapper to read shit man

	//Pattern Table
	if (address < 0x2000) {
		return g_PPUTable[address >> 0xA][address & 0x3FF];
	}

	//Name Table
	if (address < 0x3F00) {
		uint index = (address & 0xC00) >> 10;
		return g_Tables[index][address & 0x3FF];
	}

	address &= 0x1F;
	return g_Palette[address];
}

void ppu_memory_main_write(uint address, uint value)
{
	//g_MemoryRegisters.ppu_addr_bus = address;

	address &= 0x3FFF;

	//Pattern Table
	if (address < 0x2000) {
		g_PPUTable[address >> 0xA][address & 0x3FF] = value;
		return;
	}

	//Name Table
	if (address < 0x3F00) {
		int index = (address&0xC00)>>10;
		g_Tables[index][address & 0x3FF] = value;
		return;
	}

	//Palette
	uint tmp = address & 0x1F;
	uint val = value & 0x3F;

	if (tmp <= 0x10) {
		tmp &= 0xF;
		g_Palette[tmp] = val;
		g_Palette[0xC] = g_Palette[0x8] = g_Palette[0x4] = g_Palette[0x0];
		g_Palette[0x10] = g_Palette[0x1C] = g_Palette[0x18] = g_Palette[0x14] = g_Palette[0x0];
	} else {
		g_Palette[tmp] = val;
	}
}