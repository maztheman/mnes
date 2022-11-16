#include "ppu.h"
#include "ppu_registers.h"
#include "ppu_memory.h"

#include <cstring>
#include <queue>
#include <vector>

#include <gfx/application.h>

#include <common/control_flags.h>
#include <common/File.h>
#include <common/global.h>

#include <mappers/mapper.h>

#include <cpu/cpu_registers.h>
#include <cpu/memory.h>
#include <cpu/memory_registers.h>

#include <filesystem>
#include <fstream>

extern void UpdateTextureFromPPU();

struct spritebmp_t 
{
	uint32_t reg;
	void set(uint32_t value) {
		reg = value & 0xFF;
	}
	void shift() {
		//clear top bit, then shift
		reg &= 0x7F;
		reg <<= 1;
	}
	uint8_t fetch() {
		return static_cast<uint8_t>((reg >> 7) & 1);
	}
};

uint8_t	g_RGBPalette[64][3] = { 0 };
uint32_t	g_aBGColor[256] = { 0 };
uint32_t	PPU_cycles = 0;

#include "ppuaddr.cpp"
#include "sprite.cpp"
#include "background.cpp"

static inline void ppu_update_scanline();

static std::vector<uint> s_arMod341(89342, 0);
static std::vector<signed int> s_arCycle2Scanline(89342, 0);
static bool s_bEvenFrame = true;

void ppu_initialize()
{
	std::filesystem::path pall = std::filesystem::current_path() / "bnes.pal";

	if (std::ifstream file(pall, std::ios::in | std::ios::binary); file)
	{
		file.read((char*)&g_RGBPalette[0][0], 192);
	}

	for (int i = 0; i < 262; i++) {
		for (int x = 0; x < 341; x++) {
			int index = i * 341 + x;
			s_arMod341[index] = x;
		}
	}

	for (int i = 0; i < 89342; i++) {
		s_arCycle2Scanline[i] = (i / 341) - 21;
	}
	printf("ppu initialize finished\n");
}

void ppu_reset()
{
	auto screenBuffer = getScreenData();
	memset(screenBuffer.data(), 0xCD, screenBuffer.size());

	PPURegs().last_2002_read = -5;
	PPURegs().scanline = 241;

	PPU_cycles = 0;
	ppu_update_scanline();
}

bool ppu_is_odd_cycle()
{
	return (PPU_cycles & 1) == 1;
}

uint ppu_get_cycle()
{
	return PPU_cycles;
}

uint ppu_get_current_scanline_cycle()
{
	return s_arMod341[PPU_cycles];
}

static void ppu_process_visible_frame(const uint& ppu_cycle)
{
	if (is_rendering_enabled()) {
		ppu_sprite_evaluation(ppu_cycle);
		if (ppu_cycle >= 1 && ppu_cycle <= 256) {
			ppu_bg_pre_process_shift_regs(ppu_cycle, 7);
			if (is_bg_enabled()) {
				ppu_bg_draw(ppu_cycle);
			}
			ppu_sprite_get_active();
			if (is_sprite_enabled()) {
				ppu_sprite_draw(ppu_cycle);
			}
			ppu_bg_post_process_shift_regs(ppu_cycle);
			ppu_sprite_post_process_regs();
		}
		if (ppu_cycle == 256) {
			inc_fine_y();
		} else if (ppu_cycle == 257) {
			set_horiz_v_from_temp();
			//ppu_bg_pre_process_shift_regs(ppu_cycle, 8);//so the reload happens
		} else if (ppu_cycle >= 321 && ppu_cycle <= 336) {
			ppu_bg_pre_process_shift_regs(ppu_cycle, 327);
			ppu_bg_post_process_shift_regs(ppu_cycle);
		} else if (ppu_cycle == 337) {
			//ppu_bg_pre_process_shift_regs(ppu_cycle, 327);
		}
	}
}

static void ppu_process_prerender_frame(const uint& ppu_cycle)
{
	if (ppu_cycle == 1) {
		clear_vblank();
		clear_sprite_zero_hit();
		clear_sprite_overflow();
	}

	if (is_rendering_enabled()) {
		if (ppu_cycle >= 1 && ppu_cycle <= 256) {
			ppu_bg_pre_process_shift_regs(ppu_cycle, 7);
			ppu_bg_post_process_shift_regs(ppu_cycle);
		}

		if (ppu_cycle >= 257 && ppu_cycle <= 320) {
			ppu_sprite_fetch_sprite_data(ppu_cycle);
		}

		if (ppu_cycle == 256) {
			inc_fine_y();
		} else if (ppu_cycle == 257) {
			set_horiz_v_from_temp();
			//ppu_bg_pre_process_shift_regs(ppu_cycle, 8);
		} else if (ppu_cycle >= 280 && ppu_cycle <= 304) {
			set_vert_v_from_temp();
		} else if (ppu_cycle >= 321 && ppu_cycle <= 336) {
			ppu_bg_pre_process_shift_regs(ppu_cycle, 327);
			ppu_bg_post_process_shift_regs(ppu_cycle);
		} else if (ppu_cycle >= 337 && ppu_cycle <= 340) {
			//unused ppu fetch
			ppu_bg_pre_process_shift_regs(ppu_cycle, 343);
		}
	}
}

static inline void ppu_update_scanline()
{
	auto scanline = s_arCycle2Scanline[PPU_cycles];//look up the scanline for this cycle

	if (scanline < 0) {
		PPURegs().scanline = 241U + static_cast<uint32_t>(21 + scanline);
	} else {
		PPURegs().scanline = static_cast<uint32_t>(scanline);
	}
}

static inline void ppu_inc_cycle()
{
	PPU_cycles++;
	if (PPU_cycles >= 89342U) {
		PPU_cycles -= 89342U;
		s_bEvenFrame = !s_bEvenFrame;
		PPURegs().last_2002_read = -5;
	}
	ppu_update_scanline();
}


void ppu_do_cycle()
{
	auto ppu_cycle = s_arMod341[PPU_cycles];//look up the supposed mod 341 of the current ppu

	if (auto scanline = PPURegs().scanline; scanline <= 239U) {
		if (ppu_cycle == 0U) {
			memset(&g_aBGColor[0], 0, sizeof(uint) * 256);//used for sprite0 hit detection
		}
		ppu_process_visible_frame(ppu_cycle);
	} else if (scanline == 241U) {
		if (ppu_cycle == 1) {
			/*
				Reading $2002 within a few PPU clocks of when VBL is set results in special-case behavior.
				Reading one PPU clock before reads it as clear and never sets the flag or generates NMI for that frame.
				Reading on the same PPU clock or one later reads it as set, clears it, and suppresses the NMI for that frame.
				Reading two or more PPU clocks before/after it's set behaves normally (reads flag's value, clears it, and doesn't affect NMI operation).
				This suppression behavior is due to the $2002 read pulling the NMI line back up too quickly after it drops (NMI is active low) for the CPU to see it.
				(CPU inputs like NMI are sampled each clock.)
			*/
			if (PPURegs().last_2002_read == PPU_cycles/* || (PPURegs().last_2002_read + 1) == PPU_cycles*/) {
				MLOG("** NMI Surpressed due to Reading of $2002 too soon before**\n");
			} else {
				set_vblank();
				if (g_MemoryRegisters.r2000 & 128) {
					g_Registers.nmi = true;
				}
			}
		}
	} else if (scanline == 261U) {
		if (ppu_cycle == 339) {
			if (s_bEvenFrame == false && is_bg_enabled()) {
				ppu_inc_cycle();
				UpdateTextureFromPPU();
			} else {
				ppu_process_prerender_frame(ppu_cycle);
			}
		} else {
			ppu_process_prerender_frame(ppu_cycle);
			if (ppu_cycle == 340) {
				UpdateTextureFromPPU();
			}
		}
	}

	current_mapper()->do_ppu_cycle();

	ppu_inc_cycle();

}
