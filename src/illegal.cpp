static inline void cpu_slo()
{
	cpu_asl();
	cpu_ora();
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

static inline void cpu_lse() {
	cpu_lsr();
	cpu_eor();
}

static inline void cpu_dcp()
{
	cpu_dec();
	cpu_cmp();
}

static inline void cpu_lax() {
	cpu_lda();
	cpu_ldx();
}

static inline void cpu_axs() {
	g_Registers.byteLatch = g_Registers.a & g_Registers.x;
}

static inline void cpu_isc() {
	cpu_inc();
	cpu_sbc();
}

static inline void cpu_alr() {
	cpu_and();
	internals::cpu_lsr(g_Registers.a);
}

static inline void cpu_arr() {
	cpu_and();
	internals::cpu_ror(g_Registers.a);
}

static inline void cpu_xaa() {
	g_Registers.a = g_Registers.x;
	cpu_and();
}

static inline void cpu_oal() {
	//ORA #$EE
	g_Registers.a |= 0xEE;
	set_nz(g_Registers.a);
	//AND $#IMM
	cpu_and();
	//TAX
	cpu_tax();
}

static inline void cpu_sax() {
	//immediate mode value used later
	uint tmp = g_Registers.byteLatch;
	//used to restore a
	uint oldA = g_Registers.a;
	//copies a into byteLatch
	cpu_sta();
	//copies x into a
	cpu_txa();
	//ands a with byteLatch (bascially a & x)
	cpu_and();
	cpu_sec();
	g_Registers.byteLatch = tmp;
	cpu_sbc();
	cpu_tax();
	//restore a
	g_Registers.byteLatch = oldA;
	cpu_lda();
}