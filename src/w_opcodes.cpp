/*
STA, STX, STY, SHA, SHX, SHY

*/

static inline void cpu_sta() 
{
	g_Registers.byteLatch = g_Registers.a;
}

static inline void cpu_stx() 
{
	g_Registers.byteLatch = g_Registers.x;
}

static inline void cpu_sty() 
{
	g_Registers.byteLatch = g_Registers.y;
}

static inline void cpu_sax() 
{
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