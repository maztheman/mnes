/*
ASL, LSR, ROL, ROR, INC, DEC,
SLO, SRE/LSE, RLA, RRA, ISB/ISC, DCP
*/
namespace {

void mnes_::cpu::internals::asl(uint32_t &value)
{
  set_carry((value & 0x80) == 0x80);
  value <<= 1;
  value &= 0xFF;
  set_nz(value);
}

void mnes_::cpu::internals::lsr(uint32_t &value)
{
  set_carry((value & 1) == 1);
  value >>= 1;
  set_nz(value);
}

void mnes_::cpu::internals::rol(uint32_t &value)
{
  value <<= 1;
  if (is_carry()) value |= 1;
  set_carry(value > 0xff);
  value &= 0xff;
  set_nz(value);
}

void mnes_::cpu::internals::ror(uint32_t &value)
{
  if (is_carry()) value |= 0x100;
  set_carry((value & 1) == 1);
  value >>= 1;
  set_nz(value);
}

void mnes_::cpu::asl() { internals::asl(cpureg.byteLatch); }

void mnes_::cpu::lsr() { internals::lsr(cpureg.byteLatch); }

void mnes_::cpu::rol() { internals::rol(cpureg.byteLatch); }

void mnes_::cpu::ror() { internals::ror(cpureg.byteLatch); }

void mnes_::cpu::inc()
{
  cpureg.byteLatch = (cpureg.byteLatch + 1) & 0xFF;
  set_nz(cpureg.byteLatch);
}

void mnes_::cpu::dec()
{
  cpureg.byteLatch = (cpureg.byteLatch - 1) & 0xFF;
  set_nz(cpureg.byteLatch);
}

void mnes_::cpu::slo()
{
  asl();
  ora();
}

void mnes_::cpu::lse()
{
  lsr();
  eor();
}

void mnes_::cpu::rla()
{
  rol();
  and_();
}

void mnes_::cpu::rra()
{
  ror();
  adc();
}

void mnes_::cpu::isc()
{
  inc();
  sbc();
}

void mnes_::cpu::dcp()
{
  dec();
  cmp();
}

}