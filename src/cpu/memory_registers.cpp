#include "memory_registers.h"

MemoryRegisters g_MemoryRegisters = {0};

void set_sprite_overflow()
{
	g_MemoryRegisters.r2002 |= 0x20;
}

void set_vblank()
{
	g_MemoryRegisters.r2002 |= 0x80;
}

bool is_vblank()
{
	return (g_MemoryRegisters.r2002 & 0x80) == 0x80;
}

void set_horiz_v_from_temp()
{
	g_MemoryRegisters.r2006 &= 0x7BE0;
	g_MemoryRegisters.r2006 |= (g_MemoryRegisters.r2006Temp & 0x41F);
	//g_MemoryRegisters.ppu_addr_bus = g_MemoryRegisters.r2006;
}

void set_vert_v_from_temp()
{
	g_MemoryRegisters.r2006 &= 0x841F;
	g_MemoryRegisters.r2006 |= (g_MemoryRegisters.r2006Temp & 0x7BE0);
	//g_MemoryRegisters.ppu_addr_bus = g_MemoryRegisters.r2006;
}

void set_sprite_zero_hit()
{
	g_MemoryRegisters.r2002 |= 0x40;
}

bool is_sprite_zero_hit()
{
	return (g_MemoryRegisters.r2002 & 0x40) == 0x40;
}

void clear_sprite_overflow()
{
	g_MemoryRegisters.r2002 &= 0xDF;
}

void clear_vblank()
{
	g_MemoryRegisters.r2002 &= 0x7F;
}

void clear_sprite_zero_hit()
{
	g_MemoryRegisters.r2002 &= 0xBF;
}

bool is_rendering_enabled()
{
	return (g_MemoryRegisters.r2001 & 0x18) != 0;
}

bool is_bg_enabled()
{
	return (g_MemoryRegisters.r2001 & 0x8) != 0;
}

bool is_sprite_enabled()
{
	return (g_MemoryRegisters.r2001 & 0x10) != 0;
}

bool show_bg_leftmost_8px()
{
	return (g_MemoryRegisters.r2001 & 0x2) == 0x2;
}

bool show_sprite_leftmost_8px()
{
	return (g_MemoryRegisters.r2001 & 0x4) == 0x4;
}

uint ppu_addr_bus()
{
	return g_MemoryRegisters.ppu_addr_bus;
}