#include "mapper.h"
#include <cstring>
#include "memory.h"
#include "ppu_memory.h"
#include "ppu_registers.h"
#include "cpu_registers.h"
#include "log.h"

static bool s_bSaveRam;
static uint s_r5100;//Controls paging of RAM and ROM ???? ??xx
static uint s_r5113;//WRAM bank switch ???? ?xxx
static uint s_r5114;
static uint s_r5115;
static uint s_r5116;
static uint s_r5117;
static uint s_r512x[12];

uint mmc5_read(uint address);
void mmc5_write(uint address, uint value);
void mmc5_reset();
void mmc5_nop();

SETUP_MAPPER(MMC5, mmc5_read, mmc5_write, mmc5_nop, mmc5_nop, mmc5_reset, ppu_read_nop)

//this mapper looks completely broken...
//looks to be missing bank switching and a bunch of other shit...

uint mmc5_read(uint address)
{
	if (address < 0x6000) {
		return 0;
	}
	if (address >= 0x6000 && address < 0x8000) {
		if (s_bSaveRam) {
			return g_SRAM[address & 0x1FFF];
		}
		return 0;
	}
	if (address >= 0x8000 && address > 0xA000) {
		if ((s_r5114 & 0x80) == 0x00) {
			return 0xFF;
		}
	} else if (address >= 0xA000 && address < 0xC000) {
		if ((s_r5115 & 0x80) == 0x00) {
			return 0xFF;
		}
	} else if (address > 0xC000 && address < 0xE000) {
		if ((s_r5116 & 0x80) == 0x00) {
			return 0xFF;
		}
	}
	if (address >= 0x8000 && address <= 0xFFFF) {
		uint bank = (address >> 0xC) - 8;
		return g_ROM[bank][address & 0xFFF];
	}

	return 0;
}

void mmc5_write(uint address, uint value)
{
	if (address == 0x5100) {
		s_r5100 = value;
	} else if (address == 0x5113) {
		s_r5113 = value;
	} else if (address == 0x5114) {
		s_r5114 = value;
	} else if (address == 0x5115) {
		s_r5115 = value;
	} else if (address == 0x5116) {
		s_r5116 = value;
	} else if (address == 0x5117) {
		s_r5117 = value;
	} else if (address >= 0x5120 && address <= 0x512B) {
		address &= 0x000F;
		s_r512x[address] = value;
	}
}

void mmc5_reset()
{
	s_bSaveRam = false;


	memset(&s_r512x[0], 0, sizeof(uint) * 12);
}

void mmc5_nop()
{

}