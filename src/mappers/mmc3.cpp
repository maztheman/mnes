#include "mmc3.h"
#include "nrom.h"

#include "mapper.h"

#include <cstring>

#include <cpu/memory.h>
#include <cpu/memory_registers.h>
#include <cpu/cpu_registers.h>
#include <cpu/memory.h>

#include <ppu/ppu_memory.h>

#include <common/Log.h>


//extern functions
extern uint		ppu_scanline();
extern void		clear_mapper1_irq();
extern void		set_mapper1_irq();
extern uint		ppu_addr_bus();
extern uint		ppu_get_current_scanline_cycle();

//Extern cpu and ppu variables

static bool		s_bSaveRam;
static bool		s_bDisableIRQ;
static bool		s_bChipEnable;
static bool		s_bForceReload;

static uint		s_r8000;
static uint		s_r8001;
static uint		s_rA000;
static uint		s_rA001;
/// <summary>
/// IRQ Counter Register
/// </summary>
static uint		s_rC000;
/// <summary>
/// IRQ Latch Register
/// </summary>
static uint		s_rC001;
static uint		s_rE000;
static uint		s_rE001;

static uint		s_nPRGCount;
static uint8_t	s_VRAM[0x2000];
static uint		s_nLastBank;
static uint		s_nScanlineCount;
static uint8_t*	s_pVROM;
static uint		s_nVromMask;
static vuchar	s_arNTRAM;

static void mmc3_write(uint address, uint value);
static void mmc3_reset();
static void mmc3_ppu_do_cycle();
static void mmc3_nop();

mapper_t& mapperMMC3()
{
	static mapper_t instance =
	{
		mapperNROM().read_memory, ppu_read_nop, mmc3_write, mmc3_nop, mmc3_ppu_do_cycle, mmc3_reset, mnes::mappers::MMC3, false
	};
	return instance;
}

//maybe split up register writes into seperate functions for clarity

static void mmc3_write(uint address, uint value)
{
	static auto& ppuTable = PPUTable();
	static auto& romData = RomData();
	auto rawData = RawData();

	//could handle writes to save ram here to keep the save ram local to mapper butt fuck it for now.

	//before sram man
	if (address < 0x6000) {
		return;
	}

	if (address < 0x8000) {
		if (s_bSaveRam) {
			SRam()[address & 0x1FFF] = value & 0xFF;
		}
		return;
	}

	//m_Log.AddLine("%04X #$%02X SL:%03ld MMC SL:%03ld", address, value, PPURegs().scanline, m_nScanlineCount);

	if (address < 0xA000) {
		bool bReg = (address & 1) == 0;
		if (bReg) {
			s_r8000 = value;
		} else {
			s_r8001 = value;
			uint nCommand = s_r8000 & 7;

			switch (nCommand) {
			case 0:
			case 1:
			{
				uint nRomBank = ((s_r8001 & s_nVromMask) * 0x400);
				uint nAddress = nCommand * 0x800;
				if ((s_r8000 & 0x80) == 0x80) {
					nAddress ^= 0x1000;
				}
				//convert to bank
				nAddress >>= 0xA;
				ppuTable[nAddress] = &s_pVROM[nRomBank];
				ppuTable[nAddress + 1] = &s_pVROM[nRomBank + 0x400];
			}
			break;
			case 2:
			case 3:
			case 4:
			case 5:
			{
				uint nRomBank = ((s_r8001 & s_nVromMask) * 0x400);
				uint nAddress = 0x1000 + ((nCommand - 2) * 0x400);
				if ((s_r8000 & 0x80) == 0x80) {
					nAddress ^= 0x1000;
				}

				//convert to bank
				nAddress >>= 0xA;
				ppuTable[nAddress] = &s_pVROM[nRomBank];
			}
			break;
			case 6:
			{
				size_t nBank = 0;
				uint nHardWire = (s_nPRGCount - 1) * 0x4000;
				if ((s_r8000 & 0x40) == 0x40) {
					romData[0] = rawData.subspan(nHardWire).data();//0x8000-9FFF is hardwires to 2nd last bank
					romData[1] = rawData.subspan(nHardWire + 0x1000).data();
					nBank = 4;//0xC000
				} else {
					romData[4] = rawData.subspan(nHardWire).data();//0xC000-DFFF is hardwires to 2nd last bank
					romData[5] = rawData.subspan(nHardWire + 0x1000).data();
					nBank = 0;//0x8000
				}

				size_t nRomBank = 0x2000 * (s_r8001 & ((s_nPRGCount * 2) - 1));
				romData[nBank] = rawData.subspan(nRomBank).data();
				romData[nBank + 1] = rawData.subspan(nRomBank + 0x1000UL).data();
			}
			break;
			case 7:
			{
				uint nRomBank = 0x2000 * (s_r8001 & ((s_nPRGCount * 2) - 1));
				romData[2] = rawData.subspan(nRomBank).data();
				romData[3] = rawData.subspan(nRomBank + 0x1000).data();
			}
			break;
			}
		}
	} else if (address < 0xC000) {
		bool bReg = (address & 1) == 0;//even
		if (bReg) {
			s_rA000 = value;
		} else {
			s_rA001 = value;
			if ((s_rA000 & 1) == 0) {
				SetVerticalMirror();
			} else {
				SetHorizontalMirror();
			}
			s_bChipEnable = (s_rA001 & 0x80) == 0x80;
			mapperMMC3().m_bSaveRam = s_bSaveRam = (s_rA001 & 0x40) == 0x0;
		}
	} else if (address < 0xE000) {
		bool bReg = (address & 1) == 0;
		if (bReg) {
			s_rC000 = value;
		} else {
			s_rC001 = value;
			s_bForceReload = true;
		}
	} else if (address < 0x10000) {
		bool bReg = (address & 1) == 0;
		if (bReg) {
			//this value doesnt seem to be used??
			s_rE000 = value;
			//Any value written here 1) disables irq
			s_bDisableIRQ = true;
			//is this how we ackknowledge interrupts?
			clear_mapper1_irq();
		} else {
			//this value doesnt seem to be used??
			s_rE001 = value;
			//any value here enables irq's
			s_bDisableIRQ = false;
		}
	}
	//m_Log.AddLine(" r8000: %02X r8001: %02X rA000: %02X rA001: %02X rC000: %02X rC001: %02X rE000: %02X rE001: %02X\n", 
	//	m_r8000, m_r8001, m_rA000, m_rA001, m_rC000, m_rC001, m_rE000, m_rE001);
}

static void SetFourScreenMirror()
{
	static auto& tables = Tables();
	auto ntRam = NTRam();
	s_arNTRAM.resize(0x800);
	tables[0] = ntRam.data();
	tables[1] = ntRam.subspan(0x400).data();
	tables[2] = &s_arNTRAM[0x000];
	tables[3] = &s_arNTRAM[0x400];
}


static void mmc3_reset()
{
	static auto& ppuTable = PPUTable();
	static auto& romData = RomData();
	auto& format = nes_format();
	auto rawData = RawData();
	//mmc3_log.Start("logs/mmc3.log");
	mapperMMC3().m_bSaveRam = s_bSaveRam = true;
	s_bDisableIRQ = false;
	s_bChipEnable = true;
	s_bForceReload = false;
	s_r8000 = 0;
	s_r8001 = 0;
	s_rA000 = 0;
	s_rA001 = 0;
	s_rC000 = 0;
	s_rE000 = 0;
	s_rE001 = 0;
	s_nScanlineCount = 241;

	clear_mapper1_irq();

	romData[0] = rawData.subspan(0x0000).data();
	romData[1] = rawData.subspan(0x1000).data();
	romData[2] = rawData.subspan(0x2000).data();
	romData[3] = rawData.subspan(0x3000).data();

	s_nPRGCount = format.prg_rom_count;

	s_nLastBank = (s_nPRGCount - 1) * 0x4000;

	romData[4] = rawData.subspan(s_nLastBank + 0x0000).data();
	romData[5] = rawData.subspan(s_nLastBank + 0x1000).data();
	romData[6] = rawData.subspan(s_nLastBank + 0x2000).data();
	romData[7] = rawData.subspan(s_nLastBank + 0x3000).data();

	if (format.chr_rom_count > 0) {
		s_nVromMask = (8 * format.chr_rom_count) - 1U;
		s_nLastBank += 0x4000;
		s_pVROM = rawData.subspan(s_nLastBank).data();
		for (uint n = 0; n < 8; n++) {
			//first 2k is swapped in at reset
			ppuTable[n] = &s_pVROM[(0x400 * n)];
		}
	} else {
		s_nVromMask = 7;
		s_pVROM = &s_VRAM[0];
		memset(&s_VRAM[0], 0, 0x2000);
		for (uint n = 0; n < 8; n++) {
			ppuTable[n] = &s_pVROM[(0x400 * n)];
		}
	}

	if ((format.rom_control_1 & 8) == 8) {
		SetFourScreenMirror();
	} else if ((format.rom_control_1 & 1) == 1) {
		SetVerticalMirror();
	} else {
		SetHorizontalMirror();
	}
}

static bool is_a12_rising(const uint s_ui_ppu_addr_last, const uint ui_ppu_addr)
{
	auto last_is_zero = (s_ui_ppu_addr_last & 0x1000) == 0;
	auto now_is_1 = (ui_ppu_addr & 0x1000) == 0x1000;
	return last_is_zero && now_is_1;
}


static void mmc3_ppu_do_cycle()
{
	static uint s_ui_ppu_addr_last = 0;

	auto ui_ppu_addr = ppu_addr_bus();

	if (ui_ppu_addr >= 0x3F00 && ui_ppu_addr <= 0x3FFF) {
		ui_ppu_addr = s_ui_ppu_addr_last;
	}

	if (is_a12_rising(s_ui_ppu_addr_last, ui_ppu_addr)) {
		//clocked
		if (s_nScanlineCount == 0 || s_bForceReload) {
			s_bForceReload = false;
			s_nScanlineCount = s_rC000;
		} else {
			s_nScanlineCount--;
		}

		if (s_nScanlineCount == 0 && s_bDisableIRQ == false) {
			set_mapper1_irq();
		}
	}
	s_ui_ppu_addr_last = ui_ppu_addr;
}

static void mmc3_nop()
{

}