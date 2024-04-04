/*
LDA, LDX, LDY, EOR, AND, ORA, ADC, SBC, CMP, BIT,
LAX, LAE, SHS, NOP
*/

static inline void cpu_lda()
{
  GRegisters().a = GRegisters().byteLatch;
  set_nz(GRegisters().a);
}

static inline void cpu_ldx()
{
  GRegisters().x = GRegisters().byteLatch;
  set_nz(GRegisters().x);
}

static inline void cpu_ldy()
{
  GRegisters().y = GRegisters().byteLatch;
  set_nz(GRegisters().y);
}

static inline void cpu_eor()
{
  GRegisters().a ^= GRegisters().byteLatch;
  set_nz(GRegisters().a);
}

static inline void cpu_and()
{
  GRegisters().a &= GRegisters().byteLatch;
  set_nz(GRegisters().a);
}

static inline void cpu_ora()
{
  GRegisters().a |= GRegisters().byteLatch;
  set_nz(GRegisters().a);
}

static inline void cpu_adc()
{
  uint temp = GRegisters().byteLatch + GRegisters().a + (is_carry() ? 1 : 0);
  SET_OVERFLOW(!((GRegisters().a ^ GRegisters().byteLatch) & 0x80) && ((GRegisters().a ^ temp) & 0x80));
  set_carry(temp > 0xff);
  GRegisters().a = temp & 0xFF;
  set_nz(GRegisters().a);
}

static inline void cpu_sbc()
{
  uint temp = GRegisters().a - GRegisters().byteLatch - (is_carry() ? 0 : 1);
  SET_OVERFLOW(
    (((GRegisters().a ^ GRegisters().byteLatch) & 0x80) == 0x80) && (((GRegisters().a ^ temp) & 0x80) == 0x80));
  set_carry(temp < 0x100);// value is 0 - 255 the carry is set else not!
  GRegisters().a = (temp & 0xff);
  set_nz(GRegisters().a);
}

static inline void cpu_cmp()
{
  uint temp = GRegisters().a - GRegisters().byteLatch;
  set_carry(temp < 0x100);
  set_nz(temp);
}

static inline void cpu_cpx()
{
  uint temp = GRegisters().x - GRegisters().byteLatch;
  set_carry(temp < 0x100);
  set_nz(temp);
}

static inline void cpu_cpy()
{
  uint temp = GRegisters().y - GRegisters().byteLatch;
  set_carry(temp < 0x100);// because its uint, anything that made it less than 0 will cause it to be larger than 0x100
  set_nz(temp);
}


static inline void cpu_bit()
{
  SET_SIGN((GRegisters().byteLatch & 0x80) == 0x80);
  SET_OVERFLOW((0x40 & GRegisters().byteLatch) == 0x40); /* Copy bit 6 to OVERFLOW flag. */
  SET_ZERO((GRegisters().byteLatch & GRegisters().a) == 0);
}

static inline void cpu_lax()
{
  cpu_lda();
  cpu_ldx();
}

static inline void cpu_nop() {}

// should be a read i think
static inline void cpu_arr()
{
  GRegisters().a &= GRegisters().byteLatch;
  GRegisters().a >>= 1;
  set_nz(GRegisters().a);
  uint b6 = (GRegisters().a & 0x80) ? 1 : 0;
  uint b5 = (GRegisters().a & 0x40) ? 1 : 0;
  set_carry(b6);
  SET_OVERFLOW(b6 ^ b5);
}

static inline void cpu_axs()
{
  GRegisters().x = ((GRegisters().a & GRegisters().x) - GRegisters().byteLatch) & 0xFF;
  set_nz(GRegisters().x);
}

static inline void cpu_alr()
{
  cpu_and();
  internals::cpu_lsr(GRegisters().a);
}


static inline void cpu_xaa() { GRegisters().a = ((GRegisters().a | 0xEE) & GRegisters().x & GRegisters().byteLatch); }

static inline void cpu_oal()
{
  // ORA #$EE
  GRegisters().a |= 0xEE;
  set_nz(GRegisters().a);
  // AND $#IMM
  cpu_and();
  // TAX
  cpu_tax();
}
