struct bitshift16_t
{
	uint reg;
	void set_hi(uint value) {
		value &= 0xFF;

		reg &= 0xFF00;
		reg |= value;
	}

	uint fetch(uint bit) {
		return (reg >> (8 + bit)) & 1;
	}

	void right_shift() {
		reg <<= 1;
		reg &= 0xFFFF;
	}
};

struct bitshift8_t
{
	uint reg;
	uint input;

	void set_input(uint value) {
		input = 0;
		input = value & 1;
	}

	void shift() {
		reg <<= 1;
		reg |= (input & 1);
		reg &= 0xFF;
	}

	uint fetch(uint bit) {
		return (reg >> bit) & 1;
	}
};

bitshift16_t test1;
bitshift16_t test2;
bitshift8_t ab1;
bitshift8_t ab2;
static uchar ppu_render_latch[4] = { 0 };

static inline uchar get_attribute_byte(uint value, uint screen)
{
	return (((value & (3 << (screen << 1))) >> (screen << 1)))/* << 2*/;
}

static inline void ppu_bg_pre_process_shift_regs(const uint ppu_cycle, const uint first_reload) {

	const uint& v = g_MemoryRegisters.r2006;

	uint tmp = ppu_cycle & 7;
	//get addr
	if (tmp == 1) {
		g_MemoryRegisters.ppu_addr_bus = 0x2000 | (v & 0xFFF);
	} else if (tmp == 3) {
		g_MemoryRegisters.ppu_addr_bus &= 0x2C00;
		g_MemoryRegisters.ppu_addr_bus += 0x3C0 + ((v >> 4) & 0x38) | ((v >> 2) & 0x7);
	} else if (tmp == 5) {
		g_MemoryRegisters.ppu_addr_bus = (g_MemoryRegisters.r2000 & 0x10) ? 0x1000 : 0x0000;
		g_MemoryRegisters.ppu_addr_bus |= (ppu_render_latch[0] << 4);
		g_MemoryRegisters.ppu_addr_bus |= ((v >> 12) & 0x7);
	} else if (tmp == 7) {
		g_MemoryRegisters.ppu_addr_bus += 8;
	}

	if (tmp == 2) {
		//get nt byte
		//uint ntable = (v >> 10) & 3;
		//ppu_render_latch[0] = g_Tables[ntable][(v & 0x3FF)];
		ppu_render_latch[0] = ppu_memory_main_read(g_MemoryRegisters.ppu_addr_bus);
	} else if (tmp == 4) {
		//get at byte
		//uint ntable = (v >> 10) & 3;
		//uint addr = 0x3C0 + ((v >> 4) & 0x38) | ((v >> 2) & 0x7);
		//uchar ab = g_Tables[ntable][addr];//ppu_memory_main_read(addr); ///g_Tables[ntable][addr];
		uchar ab = ppu_memory_main_read(g_MemoryRegisters.ppu_addr_bus);
		uint sno = (v & 0x40) >> 5 | ((v & 2) >> 1);
		ppu_render_latch[1] = get_attribute_byte(ab, sno);
	} else if (tmp == 6) {
		//get low bg tile byte
		//uint ptable = (g_MemoryRegisters.r2000 & 0x10) ? 0x1000 : 0x0000;
		//uint addr = ptable | (ppu_render_latch[0] << 4) | ((v >> 12) & 0x7);
		//ppu_render_latch[2] = ppu_memory_main_read(addr);
		ppu_render_latch[2] = ppu_memory_main_read(g_MemoryRegisters.ppu_addr_bus);
	} else if (tmp == 0) {
		//get hi bg tile byte
		//uint ptable = (g_MemoryRegisters.r2000 & 0x10) ? 0x1000 : 0x0000;
		//uint addr = ptable | (ppu_render_latch[0] << 4) | 8 | ((v >> 12) & 0x7);
		//ppu_render_latch[3] = ppu_memory_main_read(addr);
		ppu_render_latch[3] = ppu_memory_main_read(g_MemoryRegisters.ppu_addr_bus);
	}

	if (tmp == 0) {
		inc_coarse_x();
	}
}

static inline void ppu_bg_post_process_shift_regs(const uint& ppu_cycle)
{
	uint tmp = ppu_cycle & 7;
	
	test1.right_shift();
	test2.right_shift();
	ab1.shift();
	ab2.shift();

	if (tmp == 0) {
		test1.set_hi(ppu_render_latch[2]);
		test2.set_hi(ppu_render_latch[3]);
		ab1.set_input(ppu_render_latch[1] & 1);
		ab2.set_input((ppu_render_latch[1] >> 1) & 1);
	}
}

static inline void ppu_bg_draw(const uint& ppu_cycle)
{
	//drawing uses the scanline.front() to get the current lines data
	uchar *bits = g_pScreenBuffer;
	uchar *Palette = &g_Palette[0];
	uint RealX = (ppu_cycle - 1);
	uint RealY = g_PPURegisters.scanline;
	uint index = (768 * RealY) + (RealX * 3);

	bool bDraw = true;

	if (ppu_cycle < 9 && show_bg_leftmost_8px() == false) {
		bDraw = false;
	}

	if (bDraw) {
		uint bitoff = 7 - g_MemoryRegisters.TileXOffset;
		uint b0 = test1.fetch(bitoff);
		uint b1 = test2.fetch(bitoff);
		uint chrcolor = b0 | b1 << 1;
		uint b2 = ab1.fetch(bitoff);
		uint b3 = ab2.fetch(bitoff);
		uchar color = chrcolor | (b2 << 2) | (b3 << 3);

		g_aBGColor[RealX] = (chrcolor != 0) ? 1 : 0;

		bits[index + 0] = g_RGBPalette[Palette[color]][0];
		bits[index + 1] = g_RGBPalette[Palette[color]][1];
		bits[index + 2] = g_RGBPalette[Palette[color]][2];
	} else {
		g_aBGColor[RealX] = 0;
		bits[index + 0] = g_RGBPalette[Palette[0]][0];
		bits[index + 1] = g_RGBPalette[Palette[0]][1];
		bits[index + 2] = g_RGBPalette[Palette[0]][2];
	}
}