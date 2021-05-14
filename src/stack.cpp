static inline void cpu_pha() {
	memory_push_byte(g_Registers.a);
}

static inline void cpu_php() {
	memory_push_byte(g_Registers.status | 0x30);
}

static inline void cpu_pla() {
	g_Registers.a = memory_pop_byte();
	set_nz(g_Registers.a);
}

static inline void cpu_plp() {
	g_Registers.status = memory_pop_byte() & 0xCF;
}