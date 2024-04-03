#define NSPPU mnes::ppu::

namespace {
  namespace mnes_::ppu {
    ppu_registers ppuregs = { 241, NULL_LAST_READ };
  }
}

uint32_t NSPPU scanline() { return ppuregs.scanline; }

void NSPPU set_last_r2002_read(uint32_t value)
{
  ppuregs.last_2002_read = value;
}