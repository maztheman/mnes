#if 0
static inline bool is_page_crossed(uint a, uint b)
{
	if ((a & 0xFF00) != (b & 0xFF00)) {
		return true;
	}
	return false;
}

static inline void memory_immediate()
{
	g_Registers.addressLatch = g_Registers.pc + 1;
	g_Registers.byteLatch = ext_memory_read(g_Registers.addressLatch);
}

static inline void memory_zero_page()
{
	g_Registers.addressLatch = ext_memory_read(g_Registers.pc + 1);
}

static inline void memory_zero_page_x()
{
	g_Registers.addressLatch = (ext_memory_read(g_Registers.pc + 1) + g_Registers.x) & 0xFF;
}

static inline void memory_zero_page_y()
{
	g_Registers.addressLatch = (ext_memory_read(g_Registers.pc + 1) + g_Registers.y) & 0xFF;
}

static inline void memory_absolute()
{
	g_Registers.addressLatch = (ext_memory_read(g_Registers.pc + 2) << 8) | (ext_memory_read(g_Registers.pc + 1));
}

static inline void memory_absolute_indirect()
{
	uint temp = ext_memory_read(g_Registers.pc + 2) << 8;
	uint lowTemp = ext_memory_read(g_Registers.pc + 1);
	uint lowTempNext = (lowTemp + 1) & 0xFF;
	g_Registers.addressLatch = (ext_memory_read(temp | lowTempNext) << 8) | ext_memory_read(temp | lowTemp);
}

static inline void memory_absolute_x()
{
	memory_absolute();
	uint oldpc = g_Registers.addressLatch;

	//basically x is added to lo and if it wraps makes a dummy read on the locked hi byte and added lo byte
	g_Registers.addressLatch += g_Registers.x;
	g_Registers.addressLatch &= 0xFFFF;

	if (is_page_crossed(oldpc, oldpc + g_Registers.x)) {
		//dummy read is here which executes a cycle
		auto dummy = (oldpc & 0xFF00) | ((oldpc + g_Registers.x) & 0xFF);
		ext_memory_read(dummy);
	}


	//if (is_page_crossed(oldpc, g_Registers.addressLatch)) {
	//	cpu_do_cycle();
	//	g_Registers.memoryExtraCycles++;
	//}
}

static inline void memory_absolute_y()
{
	memory_absolute();
	uint oldpc = g_Registers.addressLatch;
	g_Registers.addressLatch += g_Registers.y;
	g_Registers.addressLatch &= 0xFFFF;
	if (is_page_crossed(oldpc, g_Registers.addressLatch)) {
		cpu_do_cycle();
		//this might have to a dummy read
	}
}

static inline void memory_indirect_x()
{
	memory_zero_page_x();
	g_Registers.addressLatch = (ext_memory_read((g_Registers.addressLatch + 1) & 0xFF) << 8) | ext_memory_read(g_Registers.addressLatch);
}

static inline void memory_indirect_y()
{
	memory_zero_page();
	g_Registers.addressLatch = (ext_memory_read((g_Registers.addressLatch + 1) & 0xFF) << 8) | ext_memory_read(g_Registers.addressLatch);
	uint oldpc = g_Registers.addressLatch;
	if (oldpc > 0xFF00) {
		int look = 0;
	}
	g_Registers.addressLatch += g_Registers.y;
	g_Registers.addressLatch &= 0xFFFF;
	if (is_page_crossed(oldpc, g_Registers.addressLatch)) {
		auto dummy = (oldpc & 0xFF00) | ((oldpc + g_Registers.y) & 0xFF);
		ext_memory_read(dummy);
		g_Registers.memoryExtraCycles++;
	}
}
#endif