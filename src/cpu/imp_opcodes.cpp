namespace {

void mnes_::cpu::clc() { set_carry(false); }

void mnes_::cpu::cld() { SET_DECIMAL(false); }

void mnes_::cpu::cli() { cpureg.delayed = delayed_i::no; }

void mnes_::cpu::clv() { SET_OVERFLOW(false); }

void mnes_::cpu::sec() { set_carry(true); }

void mnes_::cpu::sed()
{
  SET_DECIMAL(true);// maybe we should do nothing...cuz this is illegal on nes
}

void mnes_::cpu::sei() { cpureg.delayed = delayed_i::yes; }

void mnes_::cpu::tax()
{
  cpureg.x = cpureg.a;
  set_nz(cpureg.x);
}

void mnes_::cpu::tay()
{
  cpureg.y = cpureg.a;
  set_nz(cpureg.y);
}

void mnes_::cpu::tsx()
{
  cpureg.x = cpureg.stack;
  set_nz(cpureg.x);
}

void mnes_::cpu::txa()
{
  cpureg.a = cpureg.x;
  set_nz(cpureg.a);
}

void mnes_::cpu::txs() { cpureg.stack = cpureg.x; }

void mnes_::cpu::tya()
{
  cpureg.a = cpureg.y;
  set_nz(cpureg.a);
}

void mnes_::cpu::inx()
{
  cpureg.x = (cpureg.x + 1) & 0xFF;
  set_nz(cpureg.x);
}

void mnes_::cpu::iny()
{
  cpureg.y = (cpureg.y + 1) & 0xFF;
  set_nz(cpureg.y);
}

void mnes_::cpu::dex()
{
  cpureg.x = (cpureg.x - 1) & 0xFF;
  set_nz(cpureg.x);
}

void mnes_::cpu::dey()
{
  cpureg.y = (cpureg.y - 1) & 0xFF;
  set_nz(cpureg.y);
}


}