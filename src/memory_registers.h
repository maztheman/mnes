#pragma once

typedef unsigned int uint;

struct MemoryRegisters
{
	uint	r2000;
	uint	r2001;
	uint	r2002;
	uint	r2003;
	uint	r2006;
	bool	r2006Latch;
	uint	r2006Temp;
	uint	r2006ByteLatch;
	uint	TileXOffset;
	bool	oam_clear_reads;
	uint	ppu_addr_bus;
	uint	ppu_latch_byte;
};

extern MemoryRegisters g_MemoryRegisters;

void set_sprite_overflow();
void set_vblank();
void set_horiz_v_from_temp();
void set_sprite_zero_hit();
void set_vert_v_from_temp();

void clear_sprite_overflow();
void clear_vblank();
void clear_sprite_zero_hit();

bool is_rendering_enabled();
bool is_bg_enabled();
bool is_sprite_enabled();
bool is_vblank();
bool is_sprite_zero_hit();

bool show_bg_leftmost_8px();
bool show_sprite_leftmost_8px();

uint ppu_addr_bus();
