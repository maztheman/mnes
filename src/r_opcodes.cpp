/*
LDA, LDX, LDY, EOR, AND, ORA, ADC, SBC, CMP, BIT,
LAX, LAE, SHS, NOP
*/

static inline void cpu_lda() 
{
	g_Registers.a = g_Registers.byteLatch;
	set_nz(g_Registers.a);
}

static inline void cpu_ldx() 
{
	g_Registers.x = g_Registers.byteLatch;
	set_nz(g_Registers.x);
}

static inline void cpu_ldy() 
{
	g_Registers.y = g_Registers.byteLatch;
	set_nz(g_Registers.y);
}

static inline void cpu_eor() 
{
	g_Registers.a ^= g_Registers.byteLatch;
	set_nz(g_Registers.a);
}

static inline void cpu_and() 
{
	g_Registers.a &= g_Registers.byteLatch;
	set_nz(g_Registers.a);
}

static inline void cpu_ora() 
{
	g_Registers.a |= g_Registers.byteLatch;
	set_nz(g_Registers.a);
}

static inline void cpu_adc() 
{
	uint temp = g_Registers.byteLatch + g_Registers.a + (is_carry() ? 1 : 0);
	SET_OVERFLOW(!((g_Registers.a ^ g_Registers.byteLatch) & 0x80) && ((g_Registers.a ^ temp) & 0x80));
	set_carry(temp > 0xff);
	g_Registers.a = temp & 0xFF;
	set_nz(g_Registers.a);
}

static inline void cpu_sbc() 
{
	uint temp = g_Registers.a - g_Registers.byteLatch - (is_carry() ? 0 : 1);
	SET_OVERFLOW((((g_Registers.a ^ g_Registers.byteLatch) & 0x80) == 0x80) && (((g_Registers.a ^ temp) & 0x80) == 0x80));
	set_carry(temp < 0x100);//value is 0 - 255 the carry is set else not!
	g_Registers.a = (temp & 0xff);
	set_nz(g_Registers.a);
}

static inline void cpu_cmp() 
{
	uint temp = g_Registers.a - g_Registers.byteLatch;
	set_carry(temp < 0x100);
	set_nz(temp);
}

static inline void cpu_cpx() 
{
	uint temp = g_Registers.x - g_Registers.byteLatch;
	set_carry(temp < 0x100);
	set_nz(temp);
}

static inline void cpu_cpy() 
{
	uint temp = g_Registers.y - g_Registers.byteLatch;
	set_carry(temp < 0x100);//because its uint, anything that made it less than 0 will cause it to be larger than 0x100
	set_nz(temp);
}


static inline void cpu_bit() 
{
	SET_SIGN((g_Registers.byteLatch & 0x80) == 0x80);
	SET_OVERFLOW((0x40 & g_Registers.byteLatch) == 0x40);	/* Copy bit 6 to OVERFLOW flag. */
	SET_ZERO((g_Registers.byteLatch & g_Registers.a) == 0);
}

static inline void cpu_lax() 
{
	cpu_lda();
	cpu_ldx();
}

static inline void cpu_nop() 
{
}

//should be a read i think
static inline void cpu_arr()
{
	g_Registers.a &= g_Registers.byteLatch;
	g_Registers.a >>= 1;
	set_nz(g_Registers.a);
	uint b6 = (g_Registers.a & 0x80) ? 1 : 0;
	uint b5 = (g_Registers.a & 0x40) ? 1 : 0;
	set_carry(b6);
	SET_OVERFLOW(b6 ^ b5);

}

static inline void cpu_axs() 
{
	g_Registers.x = ((g_Registers.a & g_Registers.x) - g_Registers.byteLatch) & 0xFF;
	set_nz(g_Registers.x);
}

static inline void cpu_alr() {
	cpu_and();
	internals::cpu_lsr(g_Registers.a);
}


static inline void cpu_xaa() 
{
	g_Registers.a = ((g_Registers.a | 0xEE) & g_Registers.x & g_Registers.byteLatch);
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

