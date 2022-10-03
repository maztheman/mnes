/*
ASL, LSR, ROL, ROR, INC, DEC,
SLO, SRE/LSE, RLA, RRA, ISB/ISC, DCP
*/

namespace internals {
	static inline void cpu_asl(uint& value)
	{
		set_carry((value & 0x80) == 0x80);
		value <<= 1;
		value &= 0xFF;
		set_nz(value);
	}
}

static inline void cpu_asl() 
{
	internals::cpu_asl(g_Registers.byteLatch);
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

namespace internals {
	static inline void cpu_rol(uint& value)
	{
		value <<= 1;
		if (is_carry()) value |= 1;
		set_carry(value > 0xff);
		value &= 0xff;
		set_nz(value);
	}
}

static inline void cpu_rol() {
	internals::cpu_rol(g_Registers.byteLatch);
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

static inline void cpu_inc() {
	g_Registers.byteLatch = (g_Registers.byteLatch + 1) & 0xFF;
	set_nz(g_Registers.byteLatch);
}

static inline void cpu_dec() {
	g_Registers.byteLatch = (g_Registers.byteLatch - 1) & 0xFF;
	set_nz(g_Registers.byteLatch);
}

static inline void cpu_slo()
{
	cpu_asl();
	cpu_ora();
}

static inline void cpu_lse() {
	cpu_lsr();
	cpu_eor();
}

static inline void cpu_rla()
{
	cpu_rol();
	cpu_and();
}


static inline void cpu_rra() {
	cpu_ror();
	cpu_adc();
}

static inline void cpu_isc() {
	cpu_inc();
	cpu_sbc();
}

static inline void cpu_dcp()
{
	cpu_dec();
	cpu_cmp();
}