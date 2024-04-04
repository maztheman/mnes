static inline void cpu_clc() { set_carry(false); }

static inline void cpu_cld() { SET_DECIMAL(false); }

static inline void cpu_cli() { GRegisters().delayed = delayed_i::no; }

static inline void cpu_clv() { SET_OVERFLOW(false); }

static inline void cpu_sec() { set_carry(true); }

static inline void cpu_sed()
{
  SET_DECIMAL(true);// maybe we should do nothing...cuz this is illegal on nes
}

static inline void cpu_sei() { GRegisters().delayed = delayed_i::yes; }

static inline void cpu_tax()
{
  GRegisters().x = GRegisters().a;
  set_nz(GRegisters().x);
}

static inline void cpu_tay()
{
  GRegisters().y = GRegisters().a;
  set_nz(GRegisters().y);
}

static inline void cpu_tsx()
{
  GRegisters().x = GRegisters().stack;
  set_nz(GRegisters().x);
}

static inline void cpu_txa()
{
  GRegisters().a = GRegisters().x;
  set_nz(GRegisters().a);
}

static inline void cpu_txs() { GRegisters().stack = GRegisters().x; }

static inline void cpu_tya()
{
  GRegisters().a = GRegisters().y;
  set_nz(GRegisters().a);
}

static inline void cpu_inx()
{
  GRegisters().x = (GRegisters().x + 1) & 0xFF;
  set_nz(GRegisters().x);
}

static inline void cpu_iny()
{
  GRegisters().y = (GRegisters().y + 1) & 0xFF;
  set_nz(GRegisters().y);
}

static inline void cpu_dex()
{
  GRegisters().x = (GRegisters().x - 1) & 0xFF;
  set_nz(GRegisters().x);
}

static inline void cpu_dey()
{
  GRegisters().y = (GRegisters().y - 1) & 0xFF;
  set_nz(GRegisters().y);
}
