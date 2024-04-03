struct bitshift16_t
{
  uint reg;
  void set_hi(uint value)
  {
    value &= 0xFF;

    reg &= 0xFF00;
    reg |= value;
  }

  uint fetch(uint bit) { return (reg >> (8 + bit)) & 1; }

  void right_shift()
  {
    reg <<= 1;
    reg &= 0xFFFF;
  }
};

struct bitshift8_t
{
  uint reg;
  uint input;

  void set_input(uint value)
  {
    input = 0;
    input = value & 1;
  }

  void shift()
  {
    reg <<= 1;
    reg |= (input & 1);
    reg &= 0xFF;
  }

  uint fetch(uint bit) { return (reg >> bit) & 1; }
};

bitshift16_t test1;
bitshift16_t test2;
bitshift8_t ab1;
bitshift8_t ab2;
std::array<uint8_t, 4> ppu_render_latch = { 0 };

uint8_t get_attribute_byte(uint value, uint screen)
{
  return TO_BYTE(((value & (3 << (screen << 1))) >> (screen << 1)));
}

uint8_t ppu_memory_main_read_byte(uint address) { return TO_BYTE(pmem::read(address)); }

void ppu_bg_pre_process_shift_regs(const uint ppu_cycle, const uint)
{
  const uint32_t v = cmem::r2006();

  uint tmp = ppu_cycle & 7;
  switch(tmp) {
    //Get PPU Addr
    case 1:
      cmem::set_ppu_addr_bus(0x2000 | (v & 0xFFF));
      break;
    case 3: {
      uint32_t tmp_ppu_addr_bus = cmem::ppu_addr_bus();
      tmp_ppu_addr_bus &= 0x2C00;
      tmp_ppu_addr_bus += 0x3C0 + (((v >> 4) & 0x38) | ((v >> 2) & 0x7));
      cmem::set_ppu_addr_bus(tmp_ppu_addr_bus);
    }
      break;
    case 5: {
      uint32_t tmp_ppu_addr_bus = cmem::get_bg_pattern_table_addr();
      tmp_ppu_addr_bus |= (ppu_render_latch[0] << 4);
      tmp_ppu_addr_bus |= ((v >> 12) & 0x7);
      cmem::set_ppu_addr_bus(tmp_ppu_addr_bus);
    }
      break;
    case 7:
      cmem::inc_ppu_addr_bus(8U);
      break;
    //Do PPU Read
    case 2:
      // get nt byte
      ppu_render_latch[0] = ppu_memory_main_read_byte(cmem::ppu_addr_bus());
      break;
    case 4: {
      // get at byte
      uint8_t ab = ppu_memory_main_read_byte(cmem::ppu_addr_bus());
      uint sno = (v & 0x40) >> 5 | ((v & 2) >> 1);
      ppu_render_latch[1] = get_attribute_byte(ab, sno);
    }
      break;
    case 6:
      // get low bg tile byte
      ppu_render_latch[2] = ppu_memory_main_read_byte(cmem::ppu_addr_bus());
      break;
    case 0:
      // get hi bg tile byte
      ppu_render_latch[3] = ppu_memory_main_read_byte(cmem::ppu_addr_bus());
      cmem::inc_coarse_x();
      break;
    default:
      break;
  }
}

void ppu_bg_post_process_shift_regs(const uint &ppu_cycle)
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

void ppu_bg_draw(const uint &ppu_cycle)
{
  // drawing uses the scanline.front() to get the current lines data
  auto bits = gfx::screen_data();
  auto palette = pmem::Palette();
  uint RealX = (ppu_cycle - 1);
  uint RealY = scanline();
  uint index = (768 * RealY) + (RealX * 3);

  bool bDraw = true;

  if (ppu_cycle < 9 && cmem::show_bg_leftmost_8px() == false) { bDraw = false; }

  if (bDraw) {
    uint bitoff = 7 - cmem::tile_x_offset();
    uint b0 = test1.fetch(bitoff);
    uint b1 = test2.fetch(bitoff);
    uint chrcolor = b0 | b1 << 1;
    uint b2 = ab1.fetch(bitoff);
    uint b3 = ab2.fetch(bitoff);
    uint8_t color = TO_BYTE(chrcolor | (b2 << 2) | (b3 << 3));

    g_aBGColor[RealX] = (chrcolor != 0) ? 1 : 0;

    bits[index + 0] = g_RGBPalette[palette[color]][0];
    bits[index + 1] = g_RGBPalette[palette[color]][1];
    bits[index + 2] = g_RGBPalette[palette[color]][2];
  } else {
    g_aBGColor[RealX] = 0;
    bits[index + 0] = g_RGBPalette[palette[0]][0];
    bits[index + 1] = g_RGBPalette[palette[0]][1];
    bits[index + 2] = g_RGBPalette[palette[0]][2];
  }
}
