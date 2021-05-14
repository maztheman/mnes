static inline void cpu_brk() {
	uint stuff = 0x30;
	uint addr_lo = BRKLO;
	uint addr_hi = BRKHI;
	if (g_Registers.prev_nmi) {
		g_Registers.nmi = false;
		stuff = 0x20;
		addr_lo = NMILO;
		addr_hi = NMIHI;
	} else if (g_Registers.prev_irq) {
		stuff = 0x20;
	} else {
		g_Registers.pc += 2;
	}
	memory_push_pc();
	memory_push_byte(g_Registers.status | stuff);
	SET_INTERRUPT(true);
	g_Registers.pc = ext_memory_read(addr_lo);
	g_Registers.pc |= ext_memory_read(addr_hi) << 8;
}

static inline void cpu_nop() {
	//cpu_do_cycle();
}