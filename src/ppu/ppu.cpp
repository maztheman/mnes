#include "ppu.h"
#include "ppu_memory.h"
#include "ppu_registers.h"

#include <cstring>
#include <queue>
#include <vector>
#include <filesystem>
#include <fstream>

#include <gfx/data.h>

#include <common/control_flags.h>
#include <common/Log.h>

#include <mappers/mapper.h>

#include <cpu/cpu_registers.h>
#include <cpu/memory.h>
#include <cpu/memory_registers.h>


//Declare private functions and data
namespace {
  constexpr uint32_t PPU_CYCLE_PER_FRAME = 89342U;

  namespace mnes_::ppu {
    void update_scanline();
    constexpr uint32_t NULL_LAST_READ = static_cast<uint32_t>(-5);

    struct ppu_registers
    {
      uint32_t scanline;
      uint32_t last_2002_read;
    };

    namespace bg {

    }
    namespace sprite {

    }
  }
}

using mnes::memory::SPRRam;
using namespace mnes;
using namespace mnes::ppu;
using namespace mnes_::ppu;

namespace pmem = mnes::ppu::memory;
namespace cmem = mnes::memory;

// clang-format off
#include "ppu_registers.cpp"
// clang-format on

namespace {
struct spritebmp_t
{
  uint32_t reg;
  void set(uint32_t value) { reg = value & 0xFF; }
  void shift()
  {
    // clear top bit, then shift
    reg &= 0x7F;
    reg <<= 1;
  }
  uint8_t fetch() { return static_cast<uint8_t>((reg >> 7) & 1); }
};

uint8_t g_RGBPalette[64][3] = { 0 };
uint32_t g_aBGColor[256] = { 0 };
uint32_t PPU_cycles = 0;
std::vector<uint32_t> s_arMod341(PPU_CYCLE_PER_FRAME, 0);
std::vector<int32_t> s_arCycle2Scanline(PPU_CYCLE_PER_FRAME, 0);
bool s_bEvenFrame = true;

// clang-format off
#include "sprite.cpp"
#include "background.cpp"
// clang-format on

void ppu_process_visible_frame(const uint &ppu_cycle)
{
  if (cmem::is_rendering_enabled()) {
    ppu_sprite_evaluation(ppu_cycle);
    if (ppu_cycle >= 1 && ppu_cycle <= 256) {
      ppu_bg_pre_process_shift_regs(ppu_cycle, 7);
      if (cmem::is_bg_enabled()) { ppu_bg_draw(ppu_cycle); }
      ppu_sprite_get_active();
      if (cmem::is_sprite_enabled()) { ppu_sprite_draw(ppu_cycle); }
      ppu_bg_post_process_shift_regs(ppu_cycle);
      ppu_sprite_post_process_regs();
    }
    if (ppu_cycle == 256) {
      cmem::inc_fine_y();
    } else if (ppu_cycle == 257) {
      cmem::set_horiz_v_from_temp();
      // ppu_bg_pre_process_shift_regs(ppu_cycle, 8);//so the reload happens
    } else if (ppu_cycle >= 321 && ppu_cycle <= 336) {
      ppu_bg_pre_process_shift_regs(ppu_cycle, 327);
      ppu_bg_post_process_shift_regs(ppu_cycle);
    } else if (ppu_cycle == 337) {
      // ppu_bg_pre_process_shift_regs(ppu_cycle, 327);
    }
  }
}

void ppu_process_prerender_frame(const uint &ppu_cycle)
{
  if (ppu_cycle == 1) {
    cmem::clear_vblank();
    cmem::clear_sprite_zero_hit();
    cmem::clear_sprite_overflow();
  }

  if (cmem::is_rendering_enabled()) {
    if (ppu_cycle >= 1 && ppu_cycle <= 256) {
      ppu_bg_pre_process_shift_regs(ppu_cycle, 7);
      ppu_bg_post_process_shift_regs(ppu_cycle);
    }

    if (ppu_cycle >= 257 && ppu_cycle <= 320) { ppu_sprite_fetch_sprite_data(ppu_cycle); }

    if (ppu_cycle == 256) {
      cmem::inc_fine_y();
    } else if (ppu_cycle == 257) {
      cmem::set_horiz_v_from_temp();
      // ppu_bg_pre_process_shift_regs(ppu_cycle, 8);
    } else if (ppu_cycle >= 280 && ppu_cycle <= 304) {
      cmem::set_vert_v_from_temp();
    } else if (ppu_cycle >= 321 && ppu_cycle <= 336) {
      ppu_bg_pre_process_shift_regs(ppu_cycle, 327);
      ppu_bg_post_process_shift_regs(ppu_cycle);
    } else if (ppu_cycle >= 337 && ppu_cycle <= 340) {
      // unused ppu fetch
      ppu_bg_pre_process_shift_regs(ppu_cycle, 343);
    }
  }
}

void mnes_::ppu::update_scanline()
{
  auto scanline = s_arCycle2Scanline[PPU_cycles];// look up the scanline for this cycle

  if (scanline < 0) {
    ppuregs.scanline = 241U + static_cast<uint32_t>(21 + scanline);
  } else {
    ppuregs.scanline = static_cast<uint32_t>(scanline);
  }
}

void ppu_inc_cycle()
{
  PPU_cycles++;
  if (PPU_cycles >= PPU_CYCLE_PER_FRAME) {
    PPU_cycles -= PPU_CYCLE_PER_FRAME;
    s_bEvenFrame = !s_bEvenFrame;
    ppuregs.last_2002_read = NULL_LAST_READ;
  }
  update_scanline();
}
}

//Public functions from ppu.h
void mnes::ppu::initialize()
{
  std::filesystem::path pall = std::filesystem::current_path() / "bnes.pal";

  if (std::ifstream file(pall, std::ios::in | std::ios::binary); file) {
    file.read(reinterpret_cast<char *>(&g_RGBPalette[0][0]), 192);
  }

  for (uint32_t i = 0U; i < 262U; i++) {
    for (uint32_t x = 0U; x < 341U; x++) {
      size_t index = i * 341U + x;
      s_arMod341[index] = x;
    }
  }

  for (int32_t i = 0; auto &value : s_arCycle2Scanline) {
    value = (i / 341) - 21;
    i++;
  }
  fmt::print(stderr, "ppu initialize finished\n");
}

void mnes::ppu::reset()
{
  auto screenBuffer = gfx::screen_data();
  memset(screenBuffer.data(), 0xCD, screenBuffer.size());

  ppuregs.last_2002_read = NULL_LAST_READ;
  ppuregs.scanline = 241;

  PPU_cycles = 0;
  update_scanline();
}

bool mnes::ppu::is_odd_cycle() { return (PPU_cycles & 1) == 1; }

uint mnes::ppu::get_cycle() { return PPU_cycles; }

uint mnes::ppu::get_current_scanline_cycle() { return s_arMod341[PPU_cycles]; }

void mnes::ppu::do_cycle()
{
  auto ppu_cycle = s_arMod341[PPU_cycles];// look up the supposed mod 341 of the current ppu

  if (auto sl = scanline(); sl <= 239U) {
    if (ppu_cycle == 0U) {
      memset(&g_aBGColor[0], 0, sizeof(uint) * 256);// used for sprite0 hit detection
    }
    ppu_process_visible_frame(ppu_cycle);
  } else if (sl == 241U) {
    if (ppu_cycle == 1) {
      /*
              Reading $2002 within a few PPU clocks of when VBL is set results in special-case behavior.
              Reading one PPU clock before reads it as clear and never sets the flag or generates NMI for that frame.
              Reading on the same PPU clock or one later reads it as set, clears it, and suppresses the NMI for that
         frame. Reading two or more PPU clocks before/after it's set behaves normally (reads flag's value, clears it,
         and doesn't affect NMI operation). This suppression behavior is due to the $2002 read pulling the NMI line back
         up too quickly after it drops (NMI is active low) for the CPU to see it. (CPU inputs like NMI are sampled each
         clock.)
      */
      if (ppuregs.last_2002_read == PPU_cycles /* || (PPURegs().last_2002_read + 1) == PPU_cycles*/) {
        MLOG("** NMI Surpressed due to Reading of $2002 too soon before**\n");
      } else {
        cmem::set_vblank();
        if (cmem::is_nmi_on_vblank()) { cpu::set_nmi(); }
      }
    }
  } else if (sl == 261U) {
    if (ppu_cycle == 339) {
      if (!s_bEvenFrame && cmem::is_bg_enabled()) {
        ppu_inc_cycle();
        gfx::update_texture_from_screen_data();
      } else {
        ppu_process_prerender_frame(ppu_cycle);
      }
    } else {
      ppu_process_prerender_frame(ppu_cycle);
      if (ppu_cycle == 340) { gfx::update_texture_from_screen_data(); }
    }
  }

  mappers::current()->do_ppu_cycle();

  ppu_inc_cycle();
}
