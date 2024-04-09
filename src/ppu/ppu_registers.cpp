#define NSPPU mnes::ppu::

uint32_t NSPPU scanline() { return ppuregs.scanline; }

void NSPPU set_last_r2002_read(uint32_t value)
{
  ppuregs.last_2002_read = value;
}