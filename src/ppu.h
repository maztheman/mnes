#pragma once


#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char uchar;
typedef unsigned int uint;

void ppu_do_cycle();
void ppu_initialize();
bool ppu_is_odd_cycle();
void ppu_reset();
uint ppu_get_cycle();
uint ppu_get_current_scanline_cycle();

extern uchar* g_pScreenBuffer;
extern uchar g_RGBPalette[64][3];

struct spritebmp_t {
	uint reg;
	void set(uint value) {
		reg = value & 0xFF;
	}
	void shift() {
		//clear top bit, then shift
		reg &= 0x7F;
		reg <<= 1;
	}
	uint fetch() {
		return (reg >> 7) & 1;
	}
};

#ifdef __cplusplus
}
#endif
