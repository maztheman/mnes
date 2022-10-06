static inline void inc_coarse_x()
{
	uint& v = g_MemoryRegisters.r2006;

	if ((v & 0x001F) == 31) { // if coarse X == 31
		v &= ~0x001F;// coarse X = 0
		v ^= 0x0400;// switch horizontal nametable
	}
	else {
		v += 1;
		//is this where i pop and get new scanline_data ?, we shall see!
	}
}

static void inc_fine_y()
{
	uint& v = g_MemoryRegisters.r2006;

	if ((v & 0x7000) != 0x7000) {        // if fine Y < 7
		v += 0x1000;                      // increment fine Y
	}
	else
	{
		v &= ~0x7000;                     // fine Y = 0
		int y = (v & 0x03E0) >> 5;        // let y = coarse Y
		if (y == 29) {
			y = 0;                          // coarse Y = 0
			v ^= 0x0800;                    // switch vertical nametable
		}
		else if (y == 31) {
			y = 0;                          // coarse Y = 0, nametable not switched
		}
		else {
			y += 1;                         // increment coarse Y
		}
		v = (v & ~0x03E0) | (y << 5);     // put coarse Y back into v
	}
}