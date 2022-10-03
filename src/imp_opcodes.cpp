static inline void cpu_clc() 
{
	set_carry(false);
}

static inline void cpu_cld() 
{
	SET_DECIMAL(false);
}

static inline void cpu_cli() 
{
	g_Registers.delayed = delayed_i::no;
}

static inline void cpu_clv() {
	SET_OVERFLOW(false);
}

static inline void cpu_sec() {
	set_carry(true);
}

static inline void cpu_sed() 
{
	SET_DECIMAL(true);//maybe we should do nothing...cuz this is illegal on nes
}

static inline void cpu_sei() 
{
	g_Registers.delayed = delayed_i::yes;
}

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

static inline void cpu_inx() {
	g_Registers.x = (g_Registers.x + 1) & 0xFF;
	set_nz(g_Registers.x);
}

static inline void cpu_iny() {
	g_Registers.y = (g_Registers.y + 1) & 0xFF;
	set_nz(g_Registers.y);
}

static inline void cpu_dex() {
	g_Registers.x = (g_Registers.x - 1) & 0xFF;
	set_nz(g_Registers.x);
}

static inline void cpu_dey() {
	g_Registers.y = (g_Registers.y - 1) & 0xFF;
	set_nz(g_Registers.y);
}

