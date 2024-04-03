#include "memory_registers.h"

#define NSMEM mnes::memory::

namespace {
  namespace mnes_::memory {
    MemoryRegisters memreg;
  }

  constexpr uint32_t COARSE_X = 31U;
  constexpr uint32_t FINE_Y = 0x7000U;
  constexpr uint32_t COARSE_Y = 0x03E0U;
}

void NSMEM set_sprite_overflow() { memreg.r2002 |= 0x20; }

void NSMEM set_vblank() { memreg.r2002 |= 0x80; }

bool NSMEM is_vblank() { return (memreg.r2002 & 0x80) == 0x80; }

void NSMEM set_horiz_v_from_temp()
{
  memreg.r2006 &= 0x7BE0;
  memreg.r2006 |= (memreg.r2006Temp & 0x41F);
  // memreg.ppu_addr_bus = memreg.r2006;
}

void NSMEM set_vert_v_from_temp()
{
  memreg.r2006 &= 0x841F;
  memreg.r2006 |= (memreg.r2006Temp & 0x7BE0);
  // memreg.ppu_addr_bus = memreg.r2006;
}

void NSMEM set_sprite_zero_hit() { memreg.r2002 |= 0x40; }

bool NSMEM is_sprite_zero_hit() { return (memreg.r2002 & 0x40) == 0x40; }

bool NSMEM is_nmi_on_vblank()
{
  return !!(memreg.r2000 & 0x80);
}

void NSMEM clear_sprite_overflow() { memreg.r2002 &= 0xDF; }

void NSMEM clear_vblank() { memreg.r2002 &= 0x7F; }

void NSMEM clear_sprite_zero_hit() { memreg.r2002 &= 0xBF; }

bool NSMEM is_rendering_enabled() { return (memreg.r2001 & 0x18) != 0; }

bool NSMEM is_bg_enabled() { return (memreg.r2001 & 0x8) != 0; }

bool NSMEM is_sprite_enabled() { return (memreg.r2001 & 0x10) != 0; }

bool NSMEM show_bg_leftmost_8px() { return (memreg.r2001 & 0x2) == 0x2; }

bool NSMEM show_sprite_leftmost_8px() { return (memreg.r2001 & 0x4) == 0x4; }

uint32_t NSMEM ppu_addr_bus() { return memreg.ppu_addr_bus; }

void NSMEM inc_ppu_addr_bus(uint32_t value)
{
  memreg.ppu_addr_bus += value;
}

void NSMEM set_ppu_addr_bus(uint32_t value)
{
  memreg.ppu_addr_bus = value;
}

uint32_t NSMEM get_sprite_height()
{
  return (memreg.r2000 & 0x20) ? 16 : 8;
}

uint32_t NSMEM get_bg_pattern_table_addr()
{
  return (memreg.r2000 & 0x10) ? 0x1000 : 0x0000;
}

uint32_t NSMEM get_sprite_pattern_table_addr()
{
  return (memreg.r2000 & 0x8) ? 0x1000 : 0x0000;
}

void NSMEM inc_coarse_x()
{
  uint32_t &v = memreg.r2006;

  if ((v & COARSE_X) == 31) {// if coarse X == 31
    v &= ~COARSE_X;// coarse X = 0
    v ^= 0x0400;// switch horizontal nametable
  } else {
    v += 1;
    // is this where i pop and get new scanline_data ?, we shall see!
  }
}

void NSMEM inc_fine_y()
{
  uint32_t &v = memreg.r2006;

  if ((v & FINE_Y) != FINE_Y) {// if fine Y < 7
    v += 0x1000;// increment fine Y
  } else {
    v &= ~FINE_Y;// fine Y = 0
    uint32_t y = (v & COARSE_Y) >> 5;// let y = coarse Y
    if (y == 29) {
      y = 0;// coarse Y = 0
      v ^= 0x0800;// switch vertical nametable
    } else if (y == 31) {
      y = 0;// coarse Y = 0, nametable not switched
    } else {
      y += 1;// increment coarse Y
    }
    v = (v & ~COARSE_Y) | (y << 5U);// put coarse Y back into v
  }
}

void NSMEM set_oam_clear_reads(bool value)
{
  memreg.oam_clear_reads = value;
}

uint32_t NSMEM r2006()
{
  return memreg.r2006;
}

uint32_t NSMEM tile_x_offset()
{
  return memreg.TileXOffset;
}