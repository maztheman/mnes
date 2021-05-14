static inline void cpu_adc() {
	uint temp = g_Registers.byteLatch + g_Registers.a + (is_carry() ? 1 : 0);
	SET_OVERFLOW(!((g_Registers.a ^ g_Registers.byteLatch) & 0x80) && ((g_Registers.a ^ temp) & 0x80));
	set_carry(temp > 0xff);
	g_Registers.a = temp & 0xFF;
	set_nz(g_Registers.a);
}

static inline void cpu_sbc() {
	uint temp = g_Registers.a - g_Registers.byteLatch - (is_carry() ? 0 : 1);
	SET_OVERFLOW((((g_Registers.a ^ g_Registers.byteLatch) & 0x80) == 0x80) && (((g_Registers.a ^ temp) & 0x80) == 0x80));
	set_carry(temp < 0x100);//value is 0 - 255 the carry is set else not!
	g_Registers.a = (temp & 0xff);
	set_nz(g_Registers.a);
}

static inline void cpu_and() {
	g_Registers.a &= g_Registers.byteLatch;
	set_nz(g_Registers.a);
}

static inline void cpu_asl() {
	set_carry((g_Registers.byteLatch & 0x80) == 0x80);
	g_Registers.byteLatch <<= 1;
	g_Registers.byteLatch &= 0xFF;
	set_nz(g_Registers.byteLatch);
}

static inline void cpu_ora() {
	g_Registers.a |= g_Registers.byteLatch;
	set_nz(g_Registers.a);
}


static inline void cpu_eor() {
	g_Registers.a ^= g_Registers.byteLatch;
	set_nz(g_Registers.a);
}

static inline void cpu_rol() {
	g_Registers.byteLatch <<= 1;
	if (is_carry()) g_Registers.byteLatch |= 1;
	set_carry(g_Registers.byteLatch > 0xff);
	g_Registers.byteLatch &= 0xff;
	set_nz(g_Registers.byteLatch);
}

namespace internals {
	static inline void cpu_ror(uint& value) {
		if (is_carry()) value |= 0x100;
		set_carry((value & 1) == 1);
		value >>= 1;
		set_nz(value);
	}
}

static inline void cpu_ror() {
	internals::cpu_ror(g_Registers.byteLatch);
}

namespace internals {
	static inline void cpu_lsr(uint& value) {
		set_carry((value & 1) == 1);
		value >>= 1;
		set_nz(value);
	}
}
static inline void cpu_lsr() {
	internals::cpu_lsr(g_Registers.byteLatch);
}
