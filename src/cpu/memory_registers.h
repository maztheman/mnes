#pragma once

namespace mnes::memory {

//public memory register functions

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
bool is_nmi_on_vblank();

bool show_bg_leftmost_8px();
bool show_sprite_leftmost_8px();

uint32_t ppu_addr_bus();
void inc_ppu_addr_bus(uint32_t value);
void set_ppu_addr_bus(uint32_t value);

uint32_t get_sprite_height();
/// @brief get the pattern table address for the background
/// @return will return 0x0000 or 0x1000, depending on r2000 bit 4
uint32_t get_bg_pattern_table_addr();

/// @brief get the pattern table address for the sprites
/// @return will return 0x0000 or 0x1000, depending on r2000 bit 3
uint32_t get_sprite_pattern_table_addr();

void set_oam_clear_reads(bool value);

uint32_t r2006();
uint32_t tile_x_offset();

void inc_coarse_x();
void inc_fine_y();
}// namespace mnes::memory