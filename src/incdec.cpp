static inline void cpu_inc() {
	g_Registers.byteLatch = (g_Registers.byteLatch + 1) & 0xFF;
	set_nz(g_Registers.byteLatch);
}

static inline void cpu_inx() {
	g_Registers.x = (g_Registers.x + 1) & 0xFF;
	set_nz(g_Registers.x);
}

static inline void cpu_iny() {
	g_Registers.y = (g_Registers.y + 1) & 0xFF;
	set_nz(g_Registers.y);
}

static inline void cpu_dec() {
	g_Registers.byteLatch = (g_Registers.byteLatch - 1) & 0xFF;
	set_nz(g_Registers.byteLatch);
}

static inline void cpu_dex() {
	g_Registers.x = (g_Registers.x - 1) & 0xFF;
	set_nz(g_Registers.x);
}

static inline void cpu_dey() {
	g_Registers.y = (g_Registers.y - 1) & 0xFF;
	set_nz(g_Registers.y);
}

