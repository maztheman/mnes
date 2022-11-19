#pragma once

struct MemoryRegisters
{
	uint32_t	r2000{0U};
	uint32_t	r2001{0U};
	uint32_t	r2002{0U};
	uint32_t	r2003{0U};
	uint32_t	r2006{0U};
	bool		r2006Latch{false};
	uint32_t	r2006Temp{0U};
	uint32_t	r2006ByteLatch{0U};
	uint32_t	TileXOffset{0U};
	bool		oam_clear_reads{false};
	uint32_t	ppu_addr_bus{0U};
	uint32_t	ppu_latch_byte{0U};
};

MemoryRegisters& GMemoryRegisters();

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

uint32_t ppu_addr_bus();
