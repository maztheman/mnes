#include "mapper.h"
#include <cstring>

#include <ppu/ppu_memory.h>
#include <ppu/ppu_registers.h>
#include <cpu/cpu_registers.h>
#include <cpu/memory.h>
#include <common/Log.h>

extern uint		ppu_addr_bus();

static bool s_bSaveRam;
static uint s_IdleCount{ 0 };
static bool s_bInFrame{ false };
static uint s_ScanLineCount = 0;
static bool s_bIRQPending = false;
static bool s_bPPUIsReading = false;
static uint s_LastAddr = ~0UL;


/// <summary>
/// PRG Rom/Ram Mode 0 - 3
/// <para>xxxx xxPP</para>
/// </summary>
static uint s_r5100;

/// <summary>
/// CHR Rom/Ram Mode 0 - 3
/// <para>xxxx xxCC</para>
/// </summary>
static uint s_r5101;

/// <summary>
/// PRG RAM Protect 1
/// <para>xxxx xxWW</para>
/// <para>WW must be 10b (2) to be writable</para>
/// </summary>
static uint s_r5102;

/// <summary>
/// PRG Ram Protect 2
/// <para>xxxx xxWW</para>
/// <para>WW must be 01b (1) to be writable</para>
/// </summary>
static uint s_r5103;

/// <summary>
/// Extended RAM Mode
/// <para>xxxx xxMM</para>
/// </summary>
static uint s_r5104;

/// <summary>
/// Nametable Mapping
/// <para>0 - On-board VRAM page 0</para>
/// <para>1 - On - board VRAM page 1</para>
/// <para>2 - Internal Expansion RAM, only if the Extended RAM mode allows it($5104 is 00 / 01); otherwise, the nametable will read as all zeros,</para>
/// <para>3 - Fill - mode data</para>
/// </summary>
static uint s_r5105;

/// <summary>
/// Fill mode tile
/// <para>TTTT TTTT</para>
/// <para>All 8 bits for a tile number for fill-mode nametable</para>
/// </summary>
static uint s_r5106;

/// <summary>
/// Fill mode color
/// <para>xxxx xxAA</para>
/// <para>AA is the attribute byte for fill-mode nametable</para>
/// </summary>
static uint s_r5107;

/// <summary>
/// PRG Ram Bank switch 
/// <para>All Modes 0x6000 (8kb)</para>
/// <para>Bit 7 ignored</para>
/// </summary>
static uint s_r5113;

/// <summary>
/// PRG Rom Bank switch
/// <para>- Mode 3 0x8000 (8kb)</para>
/// <para>- Mode 2 N/A</para>
/// <para>- Mode 1 N/A</para>
/// <para>- Mode 0 N/A</para>
/// </summary>
static uint s_r5114;

/// <summary>
/// PRG Rom Bank Switch
/// <para>- Mode 3 0xA000 (8kb)</para>
/// <para>- Mode 2 0x8000 (16kb)</para>
/// <para>- Mode 1 0x8000 (16kb)</para>
/// <para>- Mode 0 N/A </para>
/// </summary>
static uint s_r5115;

/// <summary>
/// PRG Rom Bank Switch
/// <para>- Mode 3 0xC000 (8kb)</para>
/// <para>- Mode 2 0xC000 (8kb)</para>
/// <para>- Mode 1 N/A</para>
/// <para>- Mode 0 N/A</para>
/// </summary>
static uint s_r5116;

/// <summary>
/// PRG Rom Bank Switch
/// <para>- Mode 3 0xE000 (8kb)</para>
/// <para>- Mode 2 0xE000 (8kb)</para>
/// <para>- Mode 1 0xC000 (16kb)</para>
/// <para>- Mode 0 0x8000 (32kb)</para>
/// <para>Bit 7 ignored</para>
/// </summary>
static uint s_r5117;

/// <summary>
/// CHR Rom Bank Switch (r5120 - r512B)
/// </summary>
static uint s_r512x[16];

/// <summary>
/// Upper CHR Bank bits
/// <para>xxxx xxBB</para>
/// </summary>
static uint s_r5130;

/// <summary>
/// Vertical Split Mode
/// <para>ESxW WWWW</para>
/// <para>E Enable vertical split mode</para>
/// <para>S Specify vertical split screen side (0:left; 1:right)</para>
/// <para>W Specify vertical split start/stop tile</para>
/// </summary>
static uint s_r5200;

/// <summary>
/// Vertical Split Scroll
/// <para>All eight bits specify the vertical scroll value to use in split region</para>
/// </summary>
static uint s_r5201;

/// <summary>
/// Vertical Split Bank
/// <para>All eight bits select a 4 KB CHR bank at $0000-$0FFF and $1000-$1FFF while rendering the split region.</para>
/// </summary>
static uint s_r5202;

/// <summary>
/// IRQ Scanline Compare Value
/// <para>Generate a Scanline IRQ when scanline equals this value.</para>
/// </summary>
static uint s_r5203;

/// <summary>
/// Scanline IRQ Status (Read/Write)
/// <para>w: Exxx xxxx</para>
/// <para>E - Scanline IRQ Enable flag (1=enabled)</para>
/// <para>r: SVxx xxxx</para>
/// <para>S - Scanline IRQ Pending flag</para>
/// <para>V - "In Frame" flag</para>
/// </summary>
static uint s_r5204;

/// <summary>
/// 16 Bit Multiplier 1
/// <para>w: Unsigned 8-Bit Multiplicand</para>
/// <para>r: Unsigned 16-Bit Product (Low Byte)</para>
/// </summary>
static uint s_r5205;

/// <summary>
/// 16 Bit Multipler 2
/// <para>w: Unsigned 8-Bit Multiplier</para>
/// <para>r: Unsigned 16-bit Product (High Byte)</para>
/// </summary>
static uint s_r5206;

/// <summary>
/// Expansion RAM
/// <para>Mode 0/1 - Not readable (returns open bus), can only be written while the PPU is rendering (otherwise, 0 is written)</para>
/// <para>Mode 2 - Readable and writable</para>
/// <para>Mode 3 - Read - only</para>
/// </summary>
static uchar s_r5C00[0x400];

uint mmc5_read(uint address);
void mmc5_write(uint address, uint value);
void mmc5_reset();
void mmc5_nop();
uint mmc5_read_ppu_memory(uint address);
void mmc5_do_ppu_cycle();
void mmc5_do_cpu_cycle();

static void mmc5_set_prg_mode();
static void mmc5_set_chr_mode();
static void mmc5_setup_prg();
static uint mmc5_mode_3_read(uint address);

SETUP_MAPPER(MMC5, mmc5_read, mmc5_write, mmc5_do_cpu_cycle, mmc5_do_ppu_cycle, mmc5_reset, mmc5_read_ppu_memory)

uint mmc5_read(uint address)
{
	if (address == 0xFFFA || address == 0xFFFB) {
		s_bInFrame = false;
		s_LastAddr = ~0UL;
	}

	if (address < 0x6000) {
		return 0;
	}
	if (address >= 0x6000 && address < 0x8000) {
		//address range is not this hah


		if (s_bSaveRam) {
			return g_SRAM[address & 0x1FFF];
		}
		return 0;
	}
	if (address >= 0x8000 && address < 0xA000) {
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
		switch (s_r5100 & 3) {
		case 0:
			break;
		case 1:
			break;
		case 2:
			break;
		case 3:
			return mmc5_mode_3_read(address);

		}
	}

	return 0;
}

void mmc5_write(uint address, uint value)
{
	if (address == 0x2001 && (value & 0x18) == 0) {
		s_bInFrame = false;
		s_LastAddr = ~0UL;
	}

	if (address == 0x5100) {
		s_r5100 = value & 3;
	} else if (address == 0x5101) {
		s_r5101 = value & 3;
	} else if (address == 0x5102) {
		s_r5102 = value & 3;
	} else if (address == 0x5103) {
		s_r5103 = value & 3;
	} else if (address == 0x5104) {
		s_r5104 = value & 3;
	} else if (address == 0x5105) {
		s_r5105 = value & 0xFF;
	} else if (address == 0x5106) {
		s_r5106 = value & 0xFF;
	} else if (address == 0x5107) {
		s_r5107 = value & 0x3;
	} else if (address == 0x5113) {
		s_r5113 = value & 0xFF;
	} else if (address == 0x5114) {
		s_r5114 = value & 0xFF;
	} else if (address == 0x5115) {
		s_r5115 = value & 0xFF;
	} else if (address == 0x5116) {
		s_r5116 = value & 0xFF;
	} else if (address == 0x5117) {
		s_r5117 = value & 0xFF;
	} else if (address >= 0x5120 && address <= 0x512B) {
		address &= 0x000F;
		s_r512x[address] = value & 0xFF;
	} else if (address == 0x5130) {
		s_r5130 = value & 0xFF;
	} else if (address == 0x5200) {
		s_r5200 = value & 0xFF;
	} else if (address == 0x5201) {
		s_r5201 = value & 0xFF;
	} else if (address == 0x5202) {
		s_r5202 = value & 0xFF;
	} else if (address == 0x5203) {
		s_r5203 = value & 0xFF;
	} else if (address == 0x5204) {
		s_r5204 = value & 0xFF;
	} else if (address == 0x5205) {
		s_r5205 = value & 0xFF;
	} else if (address == 0x5206) {
		s_r5206 = value & 0xFF;
	} else if (address >= 0x5C00 && address <= 0x5FFF) {
		//
		s_r5C00[address & 0x3FF] = value & 0xFF;
	} else {
		int look = 0;
	}

	mmc5_setup_prg();

}

/*
PRG mode 0
CPU $6000-$7FFF: 8 KB switchable PRG RAM bank
CPU $8000-$FFFF: 32 KB switchable PRG ROM bank

PRG mode 1
CPU $6000-$7FFF: 8 KB switchable PRG RAM bank
CPU $8000-$BFFF: 16 KB switchable PRG ROM/RAM bank
CPU $C000-$FFFF: 16 KB switchable PRG ROM bank

PRG mode 2
CPU $6000-$7FFF: 8 KB switchable PRG RAM bank
CPU $8000-$BFFF: 16 KB switchable PRG ROM/RAM bank
CPU $C000-$DFFF: 8 KB switchable PRG ROM/RAM bank
CPU $E000-$FFFF: 8 KB switchable PRG ROM bank

PRG mode 3
CPU $6000-$7FFF: 8 KB switchable PRG RAM bank
CPU $8000-$9FFF: 8 KB switchable PRG ROM/RAM bank
CPU $A000-$BFFF: 8 KB switchable PRG ROM/RAM bank
CPU $C000-$DFFF: 8 KB switchable PRG ROM/RAM bank
CPU $E000-$FFFF: 8 KB switchable PRG ROM bank


CHR mode 0
PPU $0000-$1FFF: 8 KB switchable CHR bank
CHR mode 1
PPU $0000-$0FFF: 4 KB switchable CHR bank
PPU $1000-$1FFF: 4 KB switchable CHR bank

CHR mode 2
PPU $0000-$07FF: 2 KB switchable CHR bank
PPU $0800-$0FFF: 2 KB switchable CHR bank
PPU $1000-$17FF: 2 KB switchable CHR bank
PPU $1800-$1FFF: 2 KB switchable CHR bank

CHR mode 3
PPU $0000-$03FF: 1 KB switchable CHR bank
PPU $0400-$07FF: 1 KB switchable CHR bank
PPU $0800-$0BFF: 1 KB switchable CHR bank
PPU $0C00-$0FFF: 1 KB switchable CHR bank
PPU $1000-$13FF: 1 KB switchable CHR bank
PPU $1400-$17FF: 1 KB switchable CHR bank
PPU $1800-$1BFF: 1 KB switchable CHR bank
PPU $1C00-$1FFF: 1 KB switchable CHR bank
*/

//initialize the base mmc5 stuff here

void mmc5_reset()
{
	s_bSaveRam = false;
	
	s_r5100 = 0x3; //PRG Rom MMC5 defaulting to mode 3 at power on.
	s_r5117 = 0xFF; //games apparently think this should be 0xFF on start up
	s_r5205 = 0xFF;
	s_r5206 = 0xFF;
	mmc5_setup_prg();
	memset(&s_r512x[0], 0, sizeof(uint) * 12);
}

void mmc5_nop()
{

}

static void mmc5_do_scanline()
{

}

void mmc5_do_ppu_cycle()
{

}

void mmc5_do_cpu_cycle()
{
	if (s_bPPUIsReading) {
		s_IdleCount = 0;
	} else {
		s_IdleCount++;
		if (s_IdleCount == 3) {
			s_bInFrame = false;
			s_LastAddr = ~0UL;
		}
	}
	s_bPPUIsReading = false;
}

static void mmc5_set_prg_mode()
{
	switch (s_r5100 & 0x3) {
	case 0:
		break;
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;

	}
}

static void mmc5_set_chr_mode()
{
	switch (s_r5101 & 0x3) {
	case 0:
		break;
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;

	}
}

uint mmc5_read_ppu_memory(uint address) 
{ 
	static uint s_LastPPUAddrIdx = 0;

	if (address >= 0x2000 && address <= 0x2FFF && address == s_LastAddr) {
		s_LastPPUAddrIdx++;
		if (s_LastPPUAddrIdx == 2) {
			if (s_bInFrame == false) {
				s_bInFrame = true;
				s_ScanLineCount = 0;
			} else {
				s_ScanLineCount++;
				if (s_ScanLineCount == s_r5203) {
					s_bIRQPending = true;
				}
			}
		}
	} else {
		s_LastPPUAddrIdx = 0;
	}

	s_LastAddr = address;
	s_bPPUIsReading = true;

	return 0; 
}


/// <summary>
/// Take into account 1) curent mode 2) banks for all the spots 3) do it now
/// </summary>
static void mmc5_setup_prg()
{

}

static uint mmc5_mode_3_read(uint address)
{
	if (address < 0x8000) {
		//ram stuff

	} else if (address < 0xA000) {

	} else if (address < 0xC000) {

	} else if (address < 0xE000) {

	} else if (address < 0x10000) {
		address &= 0x1FFF;
		address |= ((s_r5117 & 0x7F) << 13);
		return g_arRawData[address];
	}

	return ~0;//basically a bad null 
}