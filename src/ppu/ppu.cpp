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

#include <inttypes.h>

using mnes::memory::SPRRam;
using namespace mnes;
using namespace mnes::ppu;

namespace pmem = mnes::ppu::memory;
namespace cmem = mnes::memory;

//Declare private functions and data
namespace {
  namespace scanline_ {
    constexpr uint32_t VISIBLE_START      = 0U;
    constexpr uint32_t VISIBLE_END        = 239U;
    constexpr uint32_t POST_RENDER_START  = 240U;
    constexpr uint32_t VBLANK             = 241U;
    constexpr uint32_t POST_RENDER_END    = 260U;
    constexpr uint32_t PRE_RENDER         = 261U;
    constexpr uint32_t COUNT              = PRE_RENDER + 1;
  }
  namespace pcycle {
    constexpr uint32_t IDLE               = 0U;

  }
  constexpr uint32_t PPU_CYCLE_PER_LINE_COUNT = 341U;
  constexpr uint32_t PPU_CYCLE_PER_FRAME_COUNT = PPU_CYCLE_PER_LINE_COUNT * scanline_::COUNT;
  
  uint8_t g_RGBPalette[64][3] = { 0 };
  std::array<uint32_t, 256> g_aBGColor = { 0 };
  uint32_t PPU_cycles = 0;
  std::vector<uint32_t> s_arMod341(PPU_CYCLE_PER_FRAME_COUNT, 0);
  std::vector<int32_t> s_arCycle2Scanline(PPU_CYCLE_PER_FRAME_COUNT, 0);
  bool s_bEvenFrame = true;

  namespace mnes_::ppu {
    void update_scanline();
    void inc_cycle();
    void process_visible_frame(const uint &ppu_cycle);
    void process_prerender_frame(const uint &ppu_cycle);

    constexpr uint32_t NULL_LAST_READ = static_cast<uint32_t>(-5);

    struct ppu_registers
    {
      uint32_t scanline;
      uint32_t last_2002_read;
    };

    ppu_registers ppuregs = { scanline_::VBLANK, NULL_LAST_READ };

    namespace bg {
#include "background.cpp"
    }
    namespace sprite {
#include "sprite.cpp"
    }
  }
}

using namespace mnes_::ppu;

// clang-format off
#include "ppu_registers.cpp"
// clang-format on

namespace {
void mnes_::ppu::process_visible_frame(const uint &ppu_cycle)
{
  if (cmem::is_rendering_enabled()) {
    sprite::evaluation(ppu_cycle);
    if (ppu_cycle >= 1 && ppu_cycle <= 256) {
      bg::pre_process_shift_regs(ppu_cycle, 7);
      if (cmem::is_bg_enabled()) { bg::draw(ppu_cycle); }
      sprite::get_active();
      if (cmem::is_sprite_enabled()) { sprite::draw(ppu_cycle); }
      bg::post_process_shift_regs(ppu_cycle);
      sprite::post_process_regs();
    }
    if (ppu_cycle == 256) {
      cmem::inc_fine_y();
    } else if (ppu_cycle == 257) {
      cmem::set_horiz_v_from_temp();
      // ppu_bg_pre_process_shift_regs(ppu_cycle, 8);//so the reload happens
    } else if (ppu_cycle >= 321 && ppu_cycle <= 336) {
      bg::pre_process_shift_regs(ppu_cycle, 327);
      bg::post_process_shift_regs(ppu_cycle);
    } else if (ppu_cycle == 337) {
      // ppu_bg_pre_process_shift_regs(ppu_cycle, 327);
    }
  }
}

void mnes_::ppu::process_prerender_frame(const uint &ppu_cycle)
{
  if (ppu_cycle == 1) {
    cmem::clear_vblank();
    cmem::clear_sprite_zero_hit();
    cmem::clear_sprite_overflow();
  }

  if (cmem::is_rendering_enabled()) {
    if (ppu_cycle >= 1 && ppu_cycle <= 256) {
      bg::pre_process_shift_regs(ppu_cycle, 7);
      bg::post_process_shift_regs(ppu_cycle);
    }

    if (ppu_cycle >= 257 && ppu_cycle <= 320) { sprite::fetch_sprite_data(ppu_cycle); }

    if (ppu_cycle == 256) {
      cmem::inc_fine_y();
    } else if (ppu_cycle == 257) {
      cmem::set_horiz_v_from_temp();
      // ppu_bg_pre_process_shift_regs(ppu_cycle, 8);
    } else if (ppu_cycle >= 280 && ppu_cycle <= 304) {
      cmem::set_vert_v_from_temp();
    } else if (ppu_cycle >= 321 && ppu_cycle <= 336) {
      bg::pre_process_shift_regs(ppu_cycle, 327);
      bg::post_process_shift_regs(ppu_cycle);
    } else if (ppu_cycle >= 337 && ppu_cycle <= 340) {
      // unused ppu fetch
      bg::pre_process_shift_regs(ppu_cycle, 343);
    }
  }
}

void mnes_::ppu::update_scanline()
{
  auto scanline = s_arCycle2Scanline[PPU_cycles];// look up the scanline for this cycle

  if (scanline < 0) {
    ppuregs.scanline = scanline_::VBLANK + static_cast<uint32_t>(21 + scanline);
  } else {
    ppuregs.scanline = static_cast<uint32_t>(scanline);
  }
}

void mnes_::ppu::inc_cycle()
{
  PPU_cycles++;
  if (PPU_cycles >= PPU_CYCLE_PER_FRAME_COUNT) {
    PPU_cycles -= PPU_CYCLE_PER_FRAME_COUNT;
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

  for (uint32_t i = 0U; i < scanline_::COUNT; i++) {
    for (uint32_t x = 0U; x < PPU_CYCLE_PER_LINE_COUNT; x++) {
      size_t index = i * PPU_CYCLE_PER_LINE_COUNT + x;
      s_arMod341[index] = x;
    }
  }

  for (int32_t i = 0; auto &value : s_arCycle2Scanline) {
    value = (i / PPU_CYCLE_PER_LINE_COUNT) - 21;
    i++;
  }

  log::main()->info("ppu initialize finished");
}

void mnes::ppu::reset()
{
  auto screenBuffer = gfx::screen_data();
  memset(screenBuffer.data(), 0xCD, screenBuffer.size());

  ppuregs.last_2002_read = NULL_LAST_READ;
  ppuregs.scanline = scanline_::VBLANK;

  PPU_cycles = 0U;
  update_scanline();
}

bool mnes::ppu::is_odd_cycle() { return (PPU_cycles & 1) == 1; }

uint32_t mnes::ppu::get_cycle() { return PPU_cycles; }

uint32_t mnes::ppu::get_current_scanline_cycle() { return s_arMod341[PPU_cycles]; }

void mnes::ppu::do_cycle()
{
  auto ppu_cycle = s_arMod341[PPU_cycles];// look up the supposed mod 341 of the current ppu

  if (auto sl = scanline(); sl <= 239U) {
    if (ppu_cycle == 0U) {
      g_aBGColor.fill(0U);
    }
    process_visible_frame(ppu_cycle);
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
        inc_cycle();
        gfx::update_texture_from_screen_data();
      } else {
        process_prerender_frame(ppu_cycle);
      }
    } else {
      process_prerender_frame(ppu_cycle);
      if (ppu_cycle == 340) { gfx::update_texture_from_screen_data(); }
    }
  }

  mappers::current()->do_ppu_cycle();

  inc_cycle();
}
