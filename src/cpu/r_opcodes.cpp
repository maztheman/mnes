/*
LDA, LDX, LDY, EOR, AND, ORA, ADC, SBC, CMP, BIT,
LAX, LAE, SHS, NOP
*/
namespace {

void mnes_::cpu::lda()
{
  cpureg.a = cpureg.byteLatch;
  set_nz(cpureg.a);
}

void mnes_::cpu::ldx()
{
  cpureg.x = cpureg.byteLatch;
  set_nz(cpureg.x);
}

void mnes_::cpu::ldy()
{
  cpureg.y = cpureg.byteLatch;
  set_nz(cpureg.y);
}

void mnes_::cpu::eor()
{
  cpureg.a ^= cpureg.byteLatch;
  set_nz(cpureg.a);
}

void mnes_::cpu::and_()
{
  cpureg.a &= cpureg.byteLatch;
  set_nz(cpureg.a);
}

void mnes_::cpu::ora()
{
  cpureg.a |= cpureg.byteLatch;
  set_nz(cpureg.a);
}

void mnes_::cpu::adc()
{
  uint temp = cpureg.byteLatch + cpureg.a + (is_carry() ? 1 : 0);
  SET_OVERFLOW(!((cpureg.a ^ cpureg.byteLatch) & 0x80) && ((cpureg.a ^ temp) & 0x80));
  set_carry(temp > 0xff);
  cpureg.a = temp & 0xFF;
  set_nz(cpureg.a);
}

void mnes_::cpu::sbc()
{
  uint temp = cpureg.a - cpureg.byteLatch - (is_carry() ? 0 : 1);
  SET_OVERFLOW(
    (((cpureg.a ^ cpureg.byteLatch) & 0x80) == 0x80) && (((cpureg.a ^ temp) & 0x80) == 0x80));
  set_carry(temp < 0x100);// value is 0 - 255 the carry is set else not!
  cpureg.a = (temp & 0xff);
  set_nz(cpureg.a);
}

void mnes_::cpu::cmp()
{
  uint temp = cpureg.a - cpureg.byteLatch;
  set_carry(temp < 0x100);
  set_nz(temp);
}

void mnes_::cpu::cpx()
{
  uint temp = cpureg.x - cpureg.byteLatch;
  set_carry(temp < 0x100);
  set_nz(temp);
}

void mnes_::cpu::cpy()
{
  uint temp = cpureg.y - cpureg.byteLatch;
  set_carry(temp < 0x100);// because its uint, anything that made it less than 0 will cause it to be larger than 0x100
  set_nz(temp);
}

void mnes_::cpu::bit()
{
  SET_SIGN((cpureg.byteLatch & 0x80) == 0x80);
  SET_OVERFLOW((0x40 & cpureg.byteLatch) == 0x40); /* Copy bit 6 to OVERFLOW flag. */
  SET_ZERO((cpureg.byteLatch & cpureg.a) == 0);
}

void mnes_::cpu::lax()
{
  lda();
  ldx();
}

void mnes_::cpu::nop() {}

// should be a read i think
void mnes_::cpu::arr()
{
  cpureg.a &= cpureg.byteLatch;
  cpureg.a >>= 1;
  set_nz(cpureg.a);
  uint b6 = (cpureg.a & 0x80) ? 1 : 0;
  uint b5 = (cpureg.a & 0x40) ? 1 : 0;
  set_carry(b6);
  SET_OVERFLOW(b6 ^ b5);
}

void mnes_::cpu::axs()
{
  cpureg.x = ((cpureg.a & cpureg.x) - cpureg.byteLatch) & 0xFF;
  set_nz(cpureg.x);
}

void mnes_::cpu::alr()
{
  and_();
  internals::lsr(cpureg.a);
}

void mnes_::cpu::xaa() { cpureg.a = ((cpureg.a | 0xEE) & cpureg.x & cpureg.byteLatch); }

void mnes_::cpu::oal()
{
  // ORA #$EE
  cpureg.a |= 0xEE;
  set_nz(cpureg.a);
  // AND $#IMM
  and_();
  // TAX
  tax();
}

}