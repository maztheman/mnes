constexpr uint32_t COARSE_X = 31U;
constexpr uint32_t FINE_Y = 0x7000U;
constexpr uint32_t COARSE_Y = 0x03E0U;

static inline void inc_coarse_x()
{
	uint& v = g_MemoryRegisters.r2006;

	if ((v & COARSE_X) == 31) { // if coarse X == 31
		v &= ~COARSE_X;// coarse X = 0
		v ^= 0x0400;// switch horizontal nametable
	}
	else 
	{
		v += 1;
		//is this where i pop and get new scanline_data ?, we shall see!
	}
}

static void inc_fine_y()
{
	uint32_t& v = g_MemoryRegisters.r2006;

	if ((v & FINE_Y) != FINE_Y) {        // if fine Y < 7
		v += 0x1000;                      // increment fine Y
	}
	else
	{
		v &= ~FINE_Y;                     // fine Y = 0
		uint32_t y = (v & COARSE_Y) >> 5;        // let y = coarse Y
		if (y == 29) 
		{
			y = 0;                          // coarse Y = 0
			v ^= 0x0800;                    // switch vertical nametable
		}
		else if (y == 31) 
		{
			y = 0;                          // coarse Y = 0, nametable not switched
		}
		else {
			y += 1;                         // increment coarse Y
		}
		v = (v & ~COARSE_Y) | (y << 5U);     // put coarse Y back into v
	}
}