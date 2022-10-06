#include <cstring>

#include "mapper.h"


#include <ppu/ppu_memory.h>
#include <ppu/ppu_registers.h>
#include <cpu/cpu_registers.h>
#include <cpu/memory.h>
#include <common/Log.h>


extern uint nrom_read(uint);

#ifdef __cplusplus
extern "C" {
#endif
	uint ppu_get_current_scanline_cycle();
#ifdef __cplusplus
}
#endif

static bool						s_bSaveRam;
static uint						s_Regs[4];
static uint						s_Latch;
static uint						s_Shift;
static uchar*					s_pVROM;
static vuchar					s_arVRAM;
static uint						s_n8KbVRomMask;
static uint						s_n4KbVRomMask;
static uint						s_n32KbPRomMask;
static uint						s_n16KbPRomMask;
static uint						s_nCartSize;
static uint						s_nMaxIRQ;
static uint						s_nIRQCounter;
static uint						s_nIRQEnable;
static uint						mmc1_last_address = ~0U;
static uint64_t					mmc1_last_cycle = 0;
static uint64_t					mmc1_mid_write_cycle = 0;


static void mmc1_set_control();
static void mmc1_set_chr_lo();
static void mmc1_set_chr_hi();
static void mmc1_set_prgrom();

void mmc1_reset();
void mmc1_write(uint, uint);
void mmc1_do_cpu_cycle();
void mmc1_nop();

SETUP_MAPPER(MMC1, nrom_read, mmc1_write, mmc1_do_cpu_cycle, mmc1_nop, mmc1_reset, ppu_read_nop)

static void MMC1SetOneScreenMirror()
{
	g_Tables[1] = g_Tables[3] = g_Tables[0] = g_Tables[2] = &g_NTRam[0x000];
}

void mmc1_reset()
{
	s_nMaxIRQ = 0x20000000 | (4 << 25);
	s_nIRQCounter = 0;
	//g_mmc1.Start("logs/mmc1.log");
	memset(&s_Regs[0], 0, sizeof(uint) * 4);
	s_Latch = 0;
	s_Shift = 0;
	s_Regs[0] = 0xC;

	g_ROM[0] = &g_arRawData[0x0000];
	g_ROM[1] = &g_arRawData[0x1000];
	g_ROM[2] = &g_arRawData[0x2000];
	g_ROM[3] = &g_arRawData[0x3000];

	uint nLastBank = (g_ines_format.prg_rom_count - 1) * 0x4000;

	g_ROM[4] = &g_arRawData[nLastBank + 0x0000];
	g_ROM[5] = &g_arRawData[nLastBank + 0x1000];
	g_ROM[6] = &g_arRawData[nLastBank + 0x2000];
	g_ROM[7] = &g_arRawData[nLastBank + 0x3000];

	s_n16KbPRomMask = (g_ines_format.prg_rom_count * 16) - 1;
	s_n32KbPRomMask = (g_ines_format.prg_rom_count * 8) - 1;

	nLastBank += 0x4000;

	if (g_ines_format.prg_rom_count < 32) {
		s_nCartSize = 0;
	} else if (g_ines_format.prg_rom_count < 64) {
		s_nCartSize = 1;
	} else {
		s_nCartSize = 2;
	}

	if (g_ines_format.chr_rom_count > 0) {
		s_n8KbVRomMask = (g_ines_format.chr_rom_count * 8) - 1;
		s_n4KbVRomMask = (g_ines_format.chr_rom_count * 16) - 1;
		s_pVROM = &g_arRawData[nLastBank];
	} else {
		s_n8KbVRomMask = 7;
		s_n4KbVRomMask = 15;
		s_arVRAM.resize(0x2000);
		s_pVROM = &s_arVRAM[0];
	}

	for (uint n = 0; n < 8; n++) {
		g_PPUTable[n] = &s_pVROM[(0x400) * n];
	}

	if ((g_ines_format.rom_control_1 & 8) == 8) {
		//SetFourScreenMirror();
	} else if ((g_ines_format.rom_control_1 & 1) == 1) {
		SetVerticalMirror();
	} else {
		SetHorizontalMirror();
	}

	maprMMC1.m_bSaveRam = s_bSaveRam = (g_ines_format.rom_control_1 & 2) == 2;
}


void mmc1_write(uint address, uint value) 
{
	if (address >= 0x6000 && address < 0x8000) {
		if (s_bSaveRam) {
			g_SRAM[address & 0x1FFF] = value & 0xFF;
		}
	}

	//save ram will be broken, fuck you.
	if (address < 0x8000) {
		return;
	}

	if (address == mmc1_last_address && mmc1_mid_write_cycle == mmc1_last_cycle) {//fetch-modify-store opcodes cause 2 writes, very quickly.  MMC1 ignores the second write (which happens to be the correct value )
		MLOG_PPU("MMC1: RWM ignored PC:$%04X SL:%ld C:%ld\n", g_Registers.pc, ppu_scanline(), ppu_get_current_scanline_cycle())
		return;
	}

	mmc1_last_address = address;
	mmc1_mid_write_cycle = mmc1_last_cycle + 1;

	if (value & 0x80) {
		s_Regs[0] = s_Regs[0] | 0xC;
	} else {
		s_Latch &= ~(1 << s_Shift);
		s_Latch |= (value & 0x1) << s_Shift++;
		if (s_Shift < 5) {
			return;
		}
		uint nRegister = (address >> 0xD) & 3;
		s_Regs[nRegister] = s_Latch & 0x1F;
	}

	MLOG_PPU("MMC1 R0:$%02X R1:$%02X R2:$%02X R3:$%02X PC:$%04X SL:%ld C:%ld T:%016lX\n", s_Regs[0], s_Regs[1], s_Regs[2], s_Regs[3], g_Registers.pc, ppu_scanline(), ppu_get_current_scanline_cycle(), g_Registers.tick_count);

	s_Shift = 0;
	s_Latch = 0;
	mmc1_set_control();
	mmc1_set_chr_lo();
	mmc1_set_chr_hi();
	mmc1_set_prgrom();

	if (s_Regs[0] & 0x10) {
		s_nIRQEnable = 0;
		s_nIRQCounter = 0;
		clear_mapper1_irq();
	} else {
		s_nIRQEnable = 1;
	}
}

static void mmc1_set_control()
{
	if ((s_Regs[0] & 2) == 2) {
		//do h/v mirrow
		if ((s_Regs[0] & 1) == 0) {
			SetVerticalMirror();
		} else {
			SetHorizontalMirror();
		}
	} else {
		//do 1 screen
		MMC1SetOneScreenMirror();
	}
}

//MMC1 R0:$1A R1:$18 R2:$00 R3:$01 PC:$A36A SL:253 C:195 T:0000000000069381
//Sprite C4: O:$0008 N:$1000 SL:0 PC:$8084 C:261 H:8 T:[$00000000000694DC]

static void mmc1_set_chr_lo()
{
	if ((s_Regs[0] & 0x10) == 0) {//switch 8k
		//low bit ignored in 8kb mode
		uint nAddress = ((s_Regs[1] & 0x1E) & s_n8KbVRomMask) * 0x2000;
		for (uint n = 0; n < 8; n++) {
			g_PPUTable[n] = &s_pVROM[nAddress + ((0x400) * n)];
		}
	} else {//switch 4k
		uint nAddress = ((s_Regs[1] & 0x1F) & s_n4KbVRomMask) * 0x1000;
		for (uint n = 0; n < 4; n++) {
			g_PPUTable[n] = &s_pVROM[nAddress + ((0x400) * n)];
		}
	}
}

//MMC1 R0:$1A R1:$18 R2:$00 R3:$01 PC:$A36A SL:253 C:195 T:0000000000069381
//Sprite C4: O:$0008 N:$1000 SL:0 PC:$8084 C:261 H:8 T:[$00000000000694DC]

static void mmc1_set_chr_hi()
{
	if ((s_Regs[0] & 0x10) == 0) {
		return;
	}
	//switch 4k at 0x1000
	uint nAddress = ((s_Regs[2] & 0x1F) & s_n4KbVRomMask) * 0x1000;
	for (uint n = 0; n < 4; n++) {
		g_PPUTable[n + 4] = &s_pVROM[nAddress + ((0x400) * n)];
	}
}

static void mmc1_set_prgrom()
{
	uint n256Bank = 0;
	if (s_nCartSize == 0) {
		n256Bank = 0;
	} else if (s_nCartSize == 1) {
		if ((s_Regs[1] & 0x10) == 0x10) {
			n256Bank = 0x40000;
		}
	} else {
		if ((s_Regs[0] & 0x10) == 0x10) {
			uint temp = (s_Regs[1] >> 4) | (s_Regs[2] >> 3);
			n256Bank = temp * 0x40000;
		} else {//if Reg0.4 == 0 then pick 1st or 3rd.
			if ((s_Regs[1] & 0x10) == 0x10) {//bank 2 (0,1,2) 2 is the third...
				n256Bank = 0x80000;
			}//else 0
		}
	}

	if ((s_Regs[0] & 8) == 0) {//switch 32kb at 0x8000
		uint nBank = ((s_Regs[3] & 0xF) >> 1) & s_n32KbPRomMask;
		uint nAddress = n256Bank + (nBank * 0x8000);
		for (uint n = 0; n < 8; n++) {
			g_ROM[n] = &g_arRawData[nAddress + (0x1000 * n)];
		}
	} else {//switch 16kb from 0x8000 or 0xC000, making one of the areas locked to first or last prom bank.
		uint nBank = (s_Regs[3] & 0xF) & s_n16KbPRomMask;
		uint nAddress = n256Bank + (nBank * 0x4000);
		
		if ((s_Regs[0] & 4) == 0x4) {//swap 0x8000, hardwire 0xC000 to end
			for (uint n = 0; n < 4; n++) {
				g_ROM[n] = &g_arRawData[nAddress + (0x1000 * n)];
			}

			uint nLastBank = (g_ines_format.prg_rom_count - 1) * 0x4000;
			if (s_nCartSize == 1 || s_nCartSize == 2) {
				nLastBank = n256Bank + 0x3C000;
			}

			g_ROM[4] = &g_arRawData[nLastBank + 0x0000];
			g_ROM[5] = &g_arRawData[nLastBank + 0x1000];
			g_ROM[6] = &g_arRawData[nLastBank + 0x2000];
			g_ROM[7] = &g_arRawData[nLastBank + 0x3000];
		} else {//swap 0xC000, hardwire 0x8000
			for (uint n = 0; n < 4; n++) {
				g_ROM[n + 4] = &g_arRawData[nAddress + (0x1000 * n)];
			}
			uint nFirstBank = 0;
			if (s_nCartSize == 1 || s_nCartSize == 2) {
				nFirstBank = n256Bank;
			}
			g_ROM[0] = &g_arRawData[nFirstBank + 0x0000];
			g_ROM[1] = &g_arRawData[nFirstBank + 0x1000];
			g_ROM[2] = &g_arRawData[nFirstBank + 0x2000];
			g_ROM[3] = &g_arRawData[nFirstBank + 0x3000];
		}
	}
}

void mmc1_do_cpu_cycle()
{
	mmc1_last_cycle++;

	if (s_nIRQEnable == 0) {
		return;
	}
	s_nIRQCounter++;
	if (s_nIRQCounter >= s_nMaxIRQ) {
		set_mapper1_irq();
	}

}

void mmc1_nop()
{

}