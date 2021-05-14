static inline void cpu_cmp() {
	uint temp = g_Registers.a - g_Registers.byteLatch;
	set_carry(temp < 0x100);
	set_nz(temp);
}

static inline void cpu_cpx() {
	uint temp = g_Registers.x - g_Registers.byteLatch;
	set_carry(temp < 0x100);
	set_nz(temp);
}

static inline void cpu_cpy() {
	uint temp = g_Registers.y - g_Registers.byteLatch;
	set_carry(temp < 0x100);//because its uint, anything that made it less than 0 will cause it to be larger than 0x100
	set_nz(temp);
}

static inline void cpu_bit() {
	SET_SIGN((g_Registers.byteLatch & 0x80) == 0x80);
	SET_OVERFLOW((0x40 & g_Registers.byteLatch) == 0x40);	/* Copy bit 6 to OVERFLOW flag. */
	SET_ZERO((g_Registers.byteLatch & g_Registers.a) == 0);
}