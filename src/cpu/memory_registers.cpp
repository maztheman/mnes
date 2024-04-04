#include "memory_registers.h"

MemoryRegisters &GMemoryRegisters()
{
  static MemoryRegisters instance;
  return instance;
}

void set_sprite_overflow() { GMemoryRegisters().r2002 |= 0x20; }

void set_vblank() { GMemoryRegisters().r2002 |= 0x80; }

bool is_vblank() { return (GMemoryRegisters().r2002 & 0x80) == 0x80; }

void set_horiz_v_from_temp()
{
  GMemoryRegisters().r2006 &= 0x7BE0;
  GMemoryRegisters().r2006 |= (GMemoryRegisters().r2006Temp & 0x41F);
  // GMemoryRegisters().ppu_addr_bus = GMemoryRegisters().r2006;
}

void set_vert_v_from_temp()
{
  GMemoryRegisters().r2006 &= 0x841F;
  GMemoryRegisters().r2006 |= (GMemoryRegisters().r2006Temp & 0x7BE0);
  // GMemoryRegisters().ppu_addr_bus = GMemoryRegisters().r2006;
}

void set_sprite_zero_hit() { GMemoryRegisters().r2002 |= 0x40; }

bool is_sprite_zero_hit() { return (GMemoryRegisters().r2002 & 0x40) == 0x40; }

void clear_sprite_overflow() { GMemoryRegisters().r2002 &= 0xDF; }

void clear_vblank() { GMemoryRegisters().r2002 &= 0x7F; }

void clear_sprite_zero_hit() { GMemoryRegisters().r2002 &= 0xBF; }

bool is_rendering_enabled() { return (GMemoryRegisters().r2001 & 0x18) != 0; }

bool is_bg_enabled() { return (GMemoryRegisters().r2001 & 0x8) != 0; }

bool is_sprite_enabled() { return (GMemoryRegisters().r2001 & 0x10) != 0; }

bool show_bg_leftmost_8px() { return (GMemoryRegisters().r2001 & 0x2) == 0x2; }

bool show_sprite_leftmost_8px() { return (GMemoryRegisters().r2001 & 0x4) == 0x4; }

uint32_t ppu_addr_bus() { return GMemoryRegisters().ppu_addr_bus; }