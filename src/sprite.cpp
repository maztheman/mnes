

signed int sprite_count[8] = { 0 };
bool sprite_active[8] = { false };
uint sprite_attribute_latch[8] = { 0 };
spritebmp_t sprite_bmp0[8];
spritebmp_t sprite_bmp1[8];

struct spritedata_t {
	uint bit0;
	uint bit1;
	uint attribute;
	uint count;
	bool active;

	void set_bit0(uint val) { bit0 = val & 0xFF; }
	void set_bit1(uint val) { bit1 = val & 0xFF; }
	void set_attribute(uint val) { attribute = val & 0xFF; }
	void set_count(uint val) { attribute = val & 0xFF; }


	uchar color() {
		uchar retval = (bit0 & 0x80) ? 1 : 0;
		retval |= (bit1 & 0x80) ? 2 : 0;
		retval |= (attribute & 3) << 2;
		return retval;
	}

	void shift() {
		bit0 &= 0x7F;
		bit1 &= 0x7F;
		bit0 <<= 1;
		bit1 <<= 1;
	}

	void decrement() {
		count--;
	}

	void check() {
		if (count == 0) {
			active = true;
		}
	}
};

struct ppu_sprite_state_t
{
	uchar	oam_latch[4];
	uint	oam_index;
	uint	oam_m_index;
	uint	oam_2nd_index;
	uint	process_type;
	bool	sprite0_in_range;

	void reset() {
		oam_index = 0;
		oam_m_index = 0;
		oam_2nd_index = 0;
		process_type = 1;
		sprite0_in_range = false;
	}

	void inc_oam_2nd_index() {
		oam_2nd_index++;
		if (oam_2nd_index == 8 && process_type == 1) {
			process_type = 2;
		}
	}

	void inc_oam_m_index(bool carry) {
		oam_m_index++;
		if (oam_m_index == 3) {
			if (carry) {
				inc_oam_index();
			}
			oam_m_index = 0;
		}
	}

	void inc_oam_m_index_bugged() {
		if (++oam_m_index >= 4) {
			oam_m_index = 0;
		}
	}

	void inc_oam_index() {
		oam_index++;
		if (oam_index == 64) {
			process_type = 3;
			oam_index = 0;
		}
	}
};

ppu_sprite_state_t	sprite_state;
spritedata_t		sprite_cache[8];
uchar				oam_secondary[32];

static inline uchar reverse_byte(uchar x)
{
	static const uchar table[] = {
		0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
		0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
		0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
		0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
		0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
		0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
		0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
		0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
		0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
		0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
		0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
		0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
		0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
		0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
		0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
		0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
		0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
		0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
		0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
		0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
		0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
		0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
		0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
		0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
		0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
		0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
		0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
		0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
		0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
		0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
		0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
		0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff,
	};
	return table[x];
}


static inline void ppu_sprite_reset_active()
{
	for (int i = 0; i < 8; i++) {
		sprite_active[i] = false;
	}
}

static inline void ppu_sprite_initalize(const uint& ppu_cycle)
{
	bool even_cycle = (ppu_cycle & 1) == 0 ? true : false;
	if (ppu_cycle == 1) {
		g_MemoryRegisters.oam_clear_reads = true;
	}

	if (!even_cycle) {
		uint idx = ppu_cycle / 2;
		oam_secondary[idx] = 0xFF;
	}
}

static inline bool is_sprite_in_range(uint scanline, uint sprite_top)
{
	uint sprite_bottom = sprite_top + ((g_MemoryRegisters.r2000 & 0x20) ? 16 : 8);
	return scanline >= sprite_top && scanline < sprite_bottom;
}

static inline void ppu_sprite_process_1()
{
	static uchar* oam2nd = &oam_secondary[0];
	uint idx = sprite_state.oam_2nd_index * 4;
	oam2nd[idx + 0] = sprite_state.oam_latch[0];
	if (is_sprite_in_range(ppu_scanline(), sprite_state.oam_latch[0])) {
		//the scanline is in range (0-7 or 0-15)
		oam2nd[idx + 1] = sprite_state.oam_latch[1];
		oam2nd[idx + 2] = sprite_state.oam_latch[2];
		oam2nd[idx + 3] = sprite_state.oam_latch[3];
		if (sprite_state.oam_index == 0) {
			sprite_state.sprite0_in_range = true;
		}
		sprite_state.inc_oam_2nd_index();
	}

	sprite_state.inc_oam_index();
}

static inline void ppu_sprite_process_2()
{
	static const uchar* oam = &g_SPR_RAM[0];
	uint idx = sprite_state.oam_index * 4;
	if (is_sprite_in_range(ppu_scanline(), oam[idx + sprite_state.oam_m_index])) {
		set_sprite_overflow();
		sprite_state.inc_oam_index();
		sprite_state.inc_oam_m_index_bugged();
		//sprite_state.inc_oam_m_index(true); //old way is to let the m_index wrap
		//sprite_state.inc_oam_m_index(true);
		//sprite_state.inc_oam_m_index(true);
	} else {
		sprite_state.inc_oam_index();
		sprite_state.inc_oam_m_index_bugged();
	}
}

static inline void ppu_sprite_process_3()
{
	sprite_state.inc_oam_index();
}


static inline void ppu_sprite_copy_to_secondary(const uint& ppu_cycle)
{
	bool even_cycle = (ppu_cycle & 1) == 0 ? true : false;
	if (ppu_cycle == 65) {
		g_MemoryRegisters.oam_clear_reads = false;
		sprite_state.reset();
	}

	if (!even_cycle) {
		static const uchar* oam = &g_SPR_RAM[0];
		memcpy(&sprite_state.oam_latch[0], oam + (4 * sprite_state.oam_index), 4);
	} else {
		if (sprite_state.process_type == 1) {
			ppu_sprite_process_1();
		} else if (sprite_state.process_type == 2) {
			ppu_sprite_process_2();
		} else if (sprite_state.process_type == 3) {
			ppu_sprite_process_3();
		}
	}
}

static inline void ppu_sprite_fetch_sprite_data(const uint& ppu_cycle)
{
	const uint tmp = ppu_cycle & 7;
	static uint selected_index = 0;
	static uchar* oam2nd = &oam_secondary[0];

	uint sprite_cycle = (ppu_cycle - 257) & 7;

	if (ppu_cycle == 257) {
		selected_index = 0;
	}

	uint idx = selected_index * 4;

	/*
	* 1. Garbage nametable byte 0,1
	* 2. Garbage nametable byte 2,3
	* 3. Pattern table tile low 4,5
	* 4. Pattern table tile high (+8 bytes from pattern table tile low) 6,7
	* 
	* 
	In addition to this, the X positions and attributes for each sprite are loaded from the secondary OAM into their respective counters/latches. 
	This happens during the second garbage nametable fetch, with the attribute byte loaded during the first tick and the X coordinate during the second.
	Garbage nametable fetch: 0 - 1
	Garbase nametable fetch: 2 - 3
	 - 2: fetch attribute byte
	 - 3: fetch x coord
	*/

#if 1
	static uint pattern_table_addr = 0;
	//use this switch eventually
	const uint cuHeight = ((g_MemoryRegisters.r2000 & 0x20) ? 16 : 8);
	//sprite cycle 0, 1, 2, 3 -- Garbage fetches
	//something must have happen in 0 and 2 but its junk
	switch (sprite_cycle)
	{
	//1. Garbage nametable byte
	case 0:
		break;
	case 1:
		break;
	//2. Garbage nametable byte
	case 2:
		//load attribute byte into latch
		break;
	case 3:
		//load x coord into latch ?
		break;
	//3. Pattern table tile low
	case 4:
		break;
	case 5:
		break;
	//4. Pattern table tile high (+8 bytes from pattern table tile low)
	case 6:
		break;
	case 7:
		break;
	}

	if (sprite_cycle == 0) {
	} else if (sprite_cycle == 1) {
		ppu_memory_main_read(g_MemoryRegisters.ppu_addr_bus);//read and throw away
	} else if (sprite_cycle == 3) {
		ppu_memory_main_read(g_MemoryRegisters.ppu_addr_bus);//read and throw away
	} else if (sprite_cycle == 4) { //sprite cycle 4, 6 set ppu_addr
		uint height = ((g_MemoryRegisters.r2000 & 0x20) ? 16 : 8);
		if (selected_index >= sprite_state.oam_2nd_index) {
			sprite_attribute_latch[selected_index] = 0xFF;
			sprite_count[selected_index] = 0xFF;

			uchar tile = 0xFF;
			uint ptable = 0;
			uint addr = 0;

			if (height == 16) {
				ptable = (tile & 1) ? 0x1000 : 0x0000;
			} else {
				ptable = (g_MemoryRegisters.r2000 & 0x8) ? 0x1000 : 0x0000;
			}
			g_MemoryRegisters.ppu_addr_bus = addr = ptable | (tile << 4);
		} else {
			uchar tile = oam2nd[idx + 1];
			uint FineY = ppu_scanline() - oam2nd[idx + 0];
			uint ptable = 0;
			sprite_attribute_latch[selected_index] = oam2nd[idx + 2];

			if (height == 16) {
				ptable = (tile & 1) ? 0x1000 : 0x0000;
				tile &= 0xFE;
				uchar bottom_tile = (FineY > 7) ? 1 : 0;
				if (sprite_attribute_latch[selected_index] & 0x80) {
					bottom_tile ^= 1;
					FineY = 15 - FineY;
				}
				tile += bottom_tile;
			} else {
				ptable = (g_MemoryRegisters.r2000 & 0x8) ? 0x1000 : 0x0000;
				if (sprite_attribute_latch[selected_index] & 0x80) {
					FineY = 7 - FineY;
				}
			}

			MLOG_PPU("Sprite C4: O:$%04X", g_MemoryRegisters.ppu_addr_bus);
			pattern_table_addr = g_MemoryRegisters.ppu_addr_bus = ptable | (tile << 4) | (FineY & 0x7);
			MLOG_PPU(" N:$%04X SL:%ld PC:$%04X C:%ld H:%ld T:[$%016lX]\n", g_MemoryRegisters.ppu_addr_bus, ppu_scanline(), g_Registers.pc, ppu_get_current_scanline_cycle(), height, g_Registers.tick_count)

			sprite_count[selected_index] = oam2nd[idx + 3];
		}
	} else if (sprite_cycle == 6) {
		const uint height = ((g_MemoryRegisters.r2000 & 0x20) ? 16 : 8);
		g_MemoryRegisters.ppu_addr_bus += 8;
		pattern_table_addr += 8;
		if (selected_index < sprite_state.oam_2nd_index) {
			MLOG_PPU("Sprite C6: A:$%04X SL:%ld PC:$%04X C:%ld H:%ld\n", g_MemoryRegisters.ppu_addr_bus, ppu_scanline(), g_Registers.pc, ppu_get_current_scanline_cycle(), height)
		}
	} else if (sprite_cycle == 5) {
		const uint height = ((g_MemoryRegisters.r2000 & 0x20) ? 16 : 8);
		if (selected_index >= sprite_state.oam_2nd_index) {
			ppu_memory_main_read(g_MemoryRegisters.ppu_addr_bus);//read and throw away
			sprite_bmp0[selected_index].set(0);
		} else {
			uchar b0 = ppu_memory_main_read(g_MemoryRegisters.ppu_addr_bus);
			MLOG_PPU("Sprite C5: A:$%04X <- $%02X SL:%ld PC:$%04X C:%ld H:%ld\n", g_MemoryRegisters.ppu_addr_bus, b0, ppu_scanline(), g_Registers.pc, ppu_get_current_scanline_cycle(), height)
			if (g_MemoryRegisters.ppu_addr_bus != pattern_table_addr) {
				MLOG_PPU("Sprite Pattern table is not the same PPU_addr:$%04X vs $%04X", g_MemoryRegisters.ppu_addr_bus, pattern_table_addr)
			}
			if (sprite_attribute_latch[selected_index] & 0x40) {
				b0 = reverse_byte(b0);
			}
			sprite_bmp0[selected_index].set(b0);
		}
	} else if (sprite_cycle == 7) {
		const uint height = ((g_MemoryRegisters.r2000 & 0x20) ? 16 : 8);
		if (selected_index >= sprite_state.oam_2nd_index) {
			ppu_memory_main_read(g_MemoryRegisters.ppu_addr_bus);//read and throw away
			sprite_bmp1[selected_index].set(0);
		} else {
			uchar b1 = ppu_memory_main_read(g_MemoryRegisters.ppu_addr_bus);
			MLOG_PPU("Sprite C7: A:$%04X <- $%02X SL:%ld PC:$%04X C:%ld H:%ld\n", g_MemoryRegisters.ppu_addr_bus, b1, ppu_scanline(), g_Registers.pc, ppu_get_current_scanline_cycle(), height)
			if (g_MemoryRegisters.ppu_addr_bus != pattern_table_addr) {
				MLOG_PPU("Sprite Pattern table is not the same PPU_addr:$%04X vs $%04X", g_MemoryRegisters.ppu_addr_bus, pattern_table_addr)
			}
			if (sprite_attribute_latch[selected_index] & 0x40) {
				b1 = reverse_byte(b1);
			}

			sprite_bmp1[selected_index].set(b1);
		}
		selected_index++;
	}
#endif

#if 0
	//sprite cycle 5, 7, read from memory

	if (tmp == 1) {

	} else if (tmp == 3) {

	} else if (tmp == 5) {

	} else if (tmp == 7) {

	}

	//tmp

	//we need to be cycle accurate for the g_MemoryRegisters.ppu_addr_bus
	//meaning 2 cycles for reading, yes, but the addr is put there on cycle 1, and finishes reading on cycle 2




	if (tmp == 0) {
		uint height = ((g_MemoryRegisters.r2000 & 0x20) ? 16 : 8);
		if (selected_index >= sprite_state.oam_2nd_index) {
			sprite_attribute_latch[selected_index] = 0xFF;
			sprite_count[selected_index] = 0xFF;

			uchar tile = 0xFF;
			uint ptable = 0;
			uint addr = 0;

			if (height == 16) {
				ptable = (tile & 1) ? 0x1000 : 0x0000;
			} else {
				ptable = (g_MemoryRegisters.r2000 & 0x8) ? 0x1000 : 0x0000;
			}

			g_MemoryRegisters.ppu_addr_bus = addr = ptable | (tile << 4);
			ppu_memory_main_read(addr);//read and throw away

			sprite_bmp0[selected_index].set(0);
			sprite_bmp1[selected_index].set(0);
		} else {
			sprite_attribute_latch[selected_index] = oam2nd[idx + 2];

			uchar tile = oam2nd[idx + 1];
			uint FineY = ppu_scanline() - oam2nd[idx + 0];
			uint ptable = 0, addr = 0, addr2 = 0;

			if (height == 16) {
				ptable = (tile & 1) ? 0x1000 : 0x0000;
				tile &= 0xFE;
				uchar bottom_tile = (FineY > 7) ? 1 : 0;
				if (sprite_attribute_latch[selected_index] & 0x80) {
					bottom_tile ^= 1;
					FineY = 15 - FineY;
				}
				tile += bottom_tile;
			} 			else {
				ptable = (g_MemoryRegisters.r2000 & 0x8) ? 0x1000 : 0x0000;
				if (sprite_attribute_latch[selected_index] & 0x80) {
					FineY = 7 - FineY;
				}
			}

			g_MemoryRegisters.ppu_addr_bus = addr = ptable | (tile << 4) | (FineY & 0x7);

			sprite_count[selected_index] = oam2nd[idx + 3];

			uchar b0 = ppu_memory_main_read(addr);
			
			g_MemoryRegisters.ppu_addr_bus = addr2 = addr | 8;

			uchar b1 = ppu_memory_main_read(addr2);

			if (sprite_attribute_latch[selected_index] & 0x40) {
				b0 = reverse_byte(b0);
				b1 = reverse_byte(b1);
			}

			sprite_bmp0[selected_index].set(b0);
			sprite_bmp1[selected_index].set(b1);
		}

		//yes index gets updated every 8 cycles
		selected_index++;
	}
#endif
}

static inline void ppu_sprite_evaluation(const uint& ppu_cycle)
{
	if (ppu_cycle == 0) {
		//placeholder if we need to init shit
		ppu_sprite_reset_active();
	} else if (ppu_cycle >= 1 && ppu_cycle <= 64) {
		ppu_sprite_initalize(ppu_cycle);
	} else if (ppu_cycle >= 65 && ppu_cycle <= 256) {
		ppu_sprite_copy_to_secondary(ppu_cycle);
	} else if (ppu_cycle >= 257 && ppu_cycle <= 320) {
		ppu_sprite_fetch_sprite_data(ppu_cycle);
	}
}

static void ppu_sprite_post_process_regs()
{
	for (int i = 0; i < 8; i++) {
		sprite_count[i]--;
		if (sprite_active[i]) {
			sprite_bmp0[i].shift();
			sprite_bmp1[i].shift();
		}
	}
}

void ppu_sprite_get_active()
{
	for (int i = 0; i < 8; i++) {
		if (!sprite_active[i]) {
			sprite_active[i] = sprite_count[i] == 0;
		}
	}
}

static inline void ppu_sprite_draw(const uint& ppu_cycle)
{
	if (ppu_cycle < 9 && show_sprite_leftmost_8px() == false) {
		return;
	}

	if (ppu_scanline() == 0) {
		return;
	}

	for (int i = 0; i < 8; i++) {
		if (sprite_active[i]) {

			uint RealX = (ppu_cycle - 1);
			uint RealY = ppu_scanline();
			uint index = (768 * RealY) + (RealX * 3);
			uchar *bits = g_pScreenBuffer;
			uchar *Palette = &g_Palette[0];

			uchar b0 = sprite_bmp0[i].fetch();
			uchar b1 = sprite_bmp1[i].fetch();
			uchar b23 = sprite_attribute_latch[i] & 3;
			uchar chrcolor = b0 | b1 << 1;
			uchar color = chrcolor | b23 << 2;

			if (i == 0 && /*sprite_state.sprite0_in_range &&*/ is_bg_enabled()) {//redundant check for bg enabled, bgcolor array should be 0..
																			 //could be sprite 0 hit
				if (g_aBGColor[RealX] == 1 && chrcolor != 0 && is_sprite_zero_hit() == false) {
					if (RealX != 255) {
						set_sprite_zero_hit();
					}
				}
			}

			//sprite has priority or, bg is transparent
			if ((sprite_attribute_latch[i] & 0x20) == 0 || g_aBGColor[RealX] == 0) {
				//is a non-transparent pixel
				if (chrcolor != 0) {
					bits[index + 0] = g_RGBPalette[Palette[0x10 + color]][0];
					bits[index + 1] = g_RGBPalette[Palette[0x10 + color]][1];
					bits[index + 2] = g_RGBPalette[Palette[0x10 + color]][2];
				}
			}

			if (chrcolor != 0) {
				break;//only first sprite to hit is hit
			}
		}
	}
}