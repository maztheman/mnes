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
	internals::cpu_asl(GRegisters().byteLatch);
}

namespace internals {
	static inline void cpu_lsr(uint& value) {
		set_carry((value & 1) == 1);
		value >>= 1;
		set_nz(value);
	}
}

static inline void cpu_lsr() {
	internals::cpu_lsr(GRegisters().byteLatch);
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
	internals::cpu_rol(GRegisters().byteLatch);
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
	internals::cpu_ror(GRegisters().byteLatch);
}

static inline void cpu_inc() {
	GRegisters().byteLatch = (GRegisters().byteLatch + 1) & 0xFF;
	set_nz(GRegisters().byteLatch);
}

static inline void cpu_dec() {
	GRegisters().byteLatch = (GRegisters().byteLatch - 1) & 0xFF;
	set_nz(GRegisters().byteLatch);
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