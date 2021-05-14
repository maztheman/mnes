static inline void cpu_lda() {
	g_Registers.a = g_Registers.byteLatch;
	set_nz(g_Registers.a);
}

static inline void cpu_ldx() {
	g_Registers.x = g_Registers.byteLatch;
	set_nz(g_Registers.x);
}

static inline void cpu_ldy() {
	g_Registers.y = g_Registers.byteLatch;
	set_nz(g_Registers.y);
}

static inline void cpu_sta() {
	g_Registers.byteLatch = g_Registers.a;
}

static inline void cpu_stx() {
	g_Registers.byteLatch = g_Registers.x;
}

static inline void cpu_sty() {
	g_Registers.byteLatch = g_Registers.y;
}


