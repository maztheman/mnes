static inline void cpu_jmp() {
	g_Registers.pc = g_Registers.addressLatch;
}

static inline void cpu_jsr() {
	g_Registers.pc += 2;
	memory_push_pc();
	g_Registers.pc = g_Registers.addressLatch;
}

static inline void cpu_rti() {
	uint temp = memory_pop_byte();
	g_Registers.status = temp & 0xCF;
	memory_pop_pc();
}

static inline void cpu_rts() {
	memory_pop_pc();
	g_Registers.pc++;
}