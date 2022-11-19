#include "mmc1.h"

#include "nrom.h"

#include "mapper.h"


#include <ppu/ppu_memory.h>
#include <ppu/ppu_registers.h>
#include <cpu/cpu_registers.h>
#include <cpu/memory.h>
#include <common/Log.h>

#include <cstring>

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
static uint8_t*					s_pVROM;
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

static void mmc1_reset();
static void mmc1_write(uint, uint);
static void mmc1_do_cpu_cycle();
static void mmc1_nop();

mapper_t& mapperMMC1()
{
	static mapper_t instance = 
	{
		mapperNROM().read_memory, ppu_read_nop, mmc1_write, mmc1_do_cpu_cycle, mmc1_nop, mmc1_reset, mnes::mappers::MMC1, false
	};
	return instance;
}

static void MMC1SetOneScreenMirror()
{
	SetOneScreenMirror();
}

static void mmc1_reset()
{
	static auto& romData = RomData();

	auto& format = nes_format();
	auto rawData = RawData();

	s_nMaxIRQ = 0x20000000 | (4 << 25);
	s_nIRQCounter = 0;
	//g_mmc1.Start("logs/mmc1.log");
	memset(&s_Regs[0], 0, sizeof(uint) * 4);
	s_Latch = 0;
	s_Shift = 0;
	s_Regs[0] = 0xCU;

	romData[0] = rawData.subspan(0x0000).data();
	romData[1] = rawData.subspan(0x1000).data();
	romData[2] = rawData.subspan(0x2000).data();
	romData[3] = rawData.subspan(0x3000).data();

	uint nLastBank = (format.prg_rom_count - 1) * 0x4000U;

	romData[4] = rawData.subspan(nLastBank + 0x0000).data();
	romData[5] = rawData.subspan(nLastBank + 0x1000).data();
	romData[6] = rawData.subspan(nLastBank + 0x2000).data();
	romData[7] = rawData.subspan(nLastBank + 0x3000).data();

	s_n16KbPRomMask = (format.prg_rom_count * 16U) - 1U;
	s_n32KbPRomMask = (format.prg_rom_count * 8U) - 1U;

	nLastBank += 0x4000U;

	if (format.prg_rom_count < 32)
	{
		s_nCartSize = 0U;
	}
	else if (format.prg_rom_count < 64)
	{
		s_nCartSize = 1U;
	}
	else
	{
		s_nCartSize = 2U;
	}

	if (format.chr_rom_count > 0)
	{
		s_n8KbVRomMask = (format.chr_rom_count * 8U) - 1U;
		s_n4KbVRomMask = (format.chr_rom_count * 16U) - 1U;
		s_pVROM = rawData.subspan(nLastBank).data();
	}
	else
	{
		s_n8KbVRomMask = 7;
		s_n4KbVRomMask = 15;
		s_arVRAM.resize(0x2000);
		s_pVROM = &s_arVRAM[0];
	}

	static auto& ppuTable = PPUTable();

	for (uint n = 0; n < 8; n++)
	{
		ppuTable[n] = &s_pVROM[(0x400) * n];
	}

	if ((format.rom_control_1 & 8) == 8)
	{
		//SetFourScreenMirror();
	}
	else if ((format.rom_control_1 & 1) == 1)
	{
		SetVerticalMirror();
	}
	else
	{
		SetHorizontalMirror();
	}

	mapperMMC1().m_bSaveRam = s_bSaveRam = (format.rom_control_1 & 2) == 2;
}


static void mmc1_write(uint address, uint value) 
{
	if (address >= 0x6000 && address < 0x8000) {
		if (s_bSaveRam) {
			SRam()[address & 0x1FFF] =  value & 0xFF;
		}
	}

	//save ram will be broken, fuck you.
	if (address < 0x8000) {
		return;
	}

	if (address == mmc1_last_address && mmc1_mid_write_cycle == mmc1_last_cycle) {//fetch-modify-store opcodes cause 2 writes, very quickly.  MMC1 ignores the second write (which happens to be the correct value )
		MLOG_PPU("MMC1: RWM ignored PC:$%04X SL:%ld C:%ld\n", GRegisters().pc, ppu_scanline(), ppu_get_current_scanline_cycle())
		return;
	}

	mmc1_last_address = address;
	mmc1_mid_write_cycle = mmc1_last_cycle + 1;

	if (value & 0x80) {
		s_Regs[0] = s_Regs[0] | 0xC;
	} else {
		s_Latch &= ~(1U << s_Shift);
		s_Latch |= (value & 0x1) << s_Shift++;
		if (s_Shift < 5) {
			return;
		}
		uint nRegister = (address >> 0xD) & 3;
		s_Regs[nRegister] = s_Latch & 0x1F;
	}

	MLOG_PPU("MMC1 R0:$%02X R1:$%02X R2:$%02X R3:$%02X PC:$%04X SL:%ld C:%ld T:%016lX\n", s_Regs[0], s_Regs[1], s_Regs[2], s_Regs[3], GRegisters().pc, ppu_scanline(), ppu_get_current_scanline_cycle(), GRegisters().tick_count);

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
	static auto& ppuTable = PPUTable();

	if ((s_Regs[0] & 0x10) == 0) {//switch 8k
		//low bit ignored in 8kb mode
		uint nAddress = ((s_Regs[1] & 0x1E) & s_n8KbVRomMask) * 0x2000;
		for (uint n = 0; n < 8; n++) {
			ppuTable[n] = &s_pVROM[nAddress + ((0x400) * n)];
		}
	} else {//switch 4k
		uint nAddress = ((s_Regs[1] & 0x1F) & s_n4KbVRomMask) * 0x1000;
		for (uint n = 0; n < 4; n++) {
			ppuTable[n] = &s_pVROM[nAddress + ((0x400) * n)];
		}
	}
}

//MMC1 R0:$1A R1:$18 R2:$00 R3:$01 PC:$A36A SL:253 C:195 T:0000000000069381
//Sprite C4: O:$0008 N:$1000 SL:0 PC:$8084 C:261 H:8 T:[$00000000000694DC]

static void mmc1_set_chr_hi()
{
	static auto& ppuTable = PPUTable();

	if ((s_Regs[0] & 0x10) == 0) {
		return;
	}
	//switch 4k at 0x1000
	uint nAddress = ((s_Regs[2] & 0x1F) & s_n4KbVRomMask) * 0x1000;
	for (uint n = 0; n < 4; n++) {
		ppuTable[n + 4] = &s_pVROM[nAddress + ((0x400) * n)];
	}
}

static void mmc1_set_prgrom()
{
	static auto& romData = RomData();
	auto& format = nes_format();
	auto rawData = RawData();

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
			romData[n] = rawData.subspan(nAddress + (0x1000 * n)).data();
		}
	} else {//switch 16kb from 0x8000 or 0xC000, making one of the areas locked to first or last prom bank.
		uint nBank = (s_Regs[3] & 0xF) & s_n16KbPRomMask;
		uint nAddress = n256Bank + (nBank * 0x4000);
		
		if ((s_Regs[0] & 4) == 0x4) {//swap 0x8000, hardwire 0xC000 to end
			for (uint n = 0; n < 4; n++) {
				romData[n] = rawData.subspan(nAddress + (0x1000 * n)).data();
			}

			uint nLastBank = (format.prg_rom_count - 1U) * 0x4000U;
			if (s_nCartSize == 1 || s_nCartSize == 2) {
				nLastBank = n256Bank + 0x3C000;
			}

			romData[4] = rawData.subspan(nLastBank + 0x0000).data();
			romData[5] = rawData.subspan(nLastBank + 0x1000).data();
			romData[6] = rawData.subspan(nLastBank + 0x2000).data();
			romData[7] = rawData.subspan(nLastBank + 0x3000).data();
		} else {//swap 0xC000, hardwire 0x8000
			for (uint n = 0; n < 4; n++)
			{
				romData[n + 4] = rawData.subspan(nAddress + (0x1000 * n)).data();
			}
			uint nFirstBank = 0;
			if (s_nCartSize == 1 || s_nCartSize == 2) {
				nFirstBank = n256Bank;
			}
			romData[0] = rawData.subspan(nFirstBank + 0x0000).data();
			romData[1] = rawData.subspan(nFirstBank + 0x1000).data();
			romData[2] = rawData.subspan(nFirstBank + 0x2000).data();
			romData[3] = rawData.subspan(nFirstBank + 0x3000).data();
		}
	}
}

static void mmc1_do_cpu_cycle()
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

static void mmc1_nop()
{

}