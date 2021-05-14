static inline void cpu_tax() {
	g_Registers.x = g_Registers.a;
	set_nz(g_Registers.x);
}

static inline void cpu_tay() {
	g_Registers.y = g_Registers.a;
	set_nz(g_Registers.y);
}

static inline void cpu_tsx() {
	g_Registers.x = g_Registers.stack;
	set_nz(g_Registers.x);
}

static inline void cpu_txa() {
	g_Registers.a = g_Registers.x;
	set_nz(g_Registers.a);
}

static inline void cpu_txs() {
	g_Registers.stack = g_Registers.x;
}

static inline void cpu_tya() {
	g_Registers.a = g_Registers.y;
	set_nz(g_Registers.a);
}