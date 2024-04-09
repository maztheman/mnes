#define NS mnes::cpu::

using namespace mnes;

namespace {
  namespace mnes_::cpu {
    Registers cpureg;
  }
}

void NS reset_registers()
{
  cpureg.tick_count = 0U;
  cpureg.delayed = delayed_i::empty;
  cpureg.a = cpureg.y = cpureg.x = 0;
  cpureg.irq = 0;
  cpureg.status = IRQ_DISABLE_FLAG_MASK;
  cpureg.stack = 0xFD;
  cpureg.pc = (memory::read(RESETLO)) | (memory::read(RESETHI) << 8);
}

uint32_t NS program_counter()
{
  return cpureg.pc;
}

uint64_t NS mnes_tick_count()
{
  return cpureg.tick_count;
}

void NS set_dpcm_irq() { cpureg.irq |= irq::dpcm; }

void NS set_apu_frame_irq() { cpureg.irq |= irq::apu_frame; }

void NS set_mapper1_irq() { cpureg.irq |= irq::mapper1; }

void NS set_mapper2_irq() { cpureg.irq |= irq::mapper2; }

void NS set_fds_irq() { cpureg.irq |= irq::fds; }

void NS set_nmi() { cpureg.nmi = true; }

void NS copy_nmi_to_prev()
{
  cpureg.prev_nmi = cpureg.nmi;
}

void NS clear_dpcm_irq() { cpureg.irq &= ~irq::dpcm; }

void NS clear_apu_frame_irq() { cpureg.irq &= ~irq::apu_frame; }

void NS clear_mapper1_irq() { cpureg.irq &= ~irq::mapper1; }

void NS clear_mapper2_irq() { cpureg.irq &= ~irq::mapper2; }

void NS clear_fds_irq() { cpureg.irq &= ~irq::fds; }

bool NS get_dpcm_irq() { return (cpureg.irq & irq::dpcm) == irq::dpcm; }

bool NS get_apu_frame_irq() { return (cpureg.irq & irq::apu_frame) == irq::apu_frame; }

bool NS get_mapper1_irq() { return (cpureg.irq & irq::mapper1) == irq::mapper1; }

bool NS get_mapper2_irq() { return (cpureg.irq & irq::mapper2) == irq::mapper2; }

bool NS get_fds_irq() { return (cpureg.irq & irq::fds) == irq::fds; }

bool NS is_carry() { return ((cpureg.status & 1) == 1); }

bool NS IF_OVERFLOW() { return ((cpureg.status & 64) == 64); }

bool NS IF_SIGN() { return ((cpureg.status & 128) == 128); }

bool NS IF_ZERO() { return ((cpureg.status & 2) == 2); }

bool NS IF_INTERRUPT() { return ((cpureg.status & 4) == 4); }

void NS set_carry(bool bValue)
{
  if (bValue) {
    cpureg.status |= 1;
  } else {
    cpureg.status &= 0xFE;
  }
}

void NS SET_OVERFLOW(bool bValue)
{
  if (bValue) {
    cpureg.status |= 64;
  } else {
    cpureg.status &= 0xBF;
  }
}

void NS SET_SIGN(bool bValue)
{
  if (bValue) {
    cpureg.status |= 128;
  } else {
    cpureg.status &= 0x7F;
  }
}

void NS SET_ZERO(bool bValue)
{
  if (bValue) {
    cpureg.status |= 2;
  } else {
    cpureg.status &= 0xFD;
  }
}

void NS SET_INTERRUPT(bool bValue)
{
  if (bValue) {
    cpureg.status |= 4;
  } else {
    cpureg.status &= 0xFB;
  }
}

void NS SET_DECIMAL(bool bValue)
{
  if (bValue) {
    cpureg.status |= 8;
  } else {
    cpureg.status &= 0xF7;
  }
}

void NS set_nz(uint32_t val)
{
  SET_SIGN((val & 0x80) == 0x80);
  SET_ZERO((val & 0xFF) == 0);
}