/*
* Read-Write-Modify Instructions
*
*
* ASL, LSR, ROL, ROR, INC, DEC,
* SLO, SRE, RLA, RRA, ISB, DCP
*
*


#   address  R/W description
--- --------- --- ------------------------------------------
1    PC       R  fetch opcode, increment PC
2    PC       R  fetch low byte of address, increment PC
3    PC       R  fetch high byte of address,
                    add index register X to low address byte,
                    increment PC
4  address+X* R  read from effective address,
                    fix the high byte of effective address
5  address+X  R  re-read from effective address
6  address+X  W  write the value back to effective address,
                    and do the operation on it
7  address+X  W  write the new value to effective address

Notes: * The high byte of the effective address may be invalid
        at this time, i.e. it may be smaller by $100.
*
*
*/
static inline void memory_rmw_absolute_indexed(const uint &indexRegister)
{
  using namespace mnes::opcodes;
  // 2
  uint pcl = ext_memory_read(GRegisters().pc++);
  // 3
  uint pch = ext_memory_read(GRegisters().pc++) << 8;
  MLOG(" $%04X, I[$%02X]", pcl | pch, indexRegister)
  pcl += indexRegister;
  // 4
  MLOG(" DR:$%04X", pch | (pcl & 0xFF))
  ext_memory_read(pch | (pcl & 0xFF));
  GRegisters().addressLatch = (pch + pcl) & 0xFFFF;
  if (GRegisters().opCode == OPCODE_LAS_AB_Y) {
    GRegisters().byteLatch = GRegisters().stack & (GRegisters().addressLatch >> 8);
    GRegisters().a = GRegisters().x = GRegisters().stack = GRegisters().byteLatch;
    set_nz(GRegisters().a);
    return;// odd behavior skips the rest
  }
  // 5
  GRegisters().byteLatch = ext_memory_read(GRegisters().addressLatch);
  MLOG(" R/W:$%04X <- $%02X", GRegisters().addressLatch, GRegisters().byteLatch)
  // 6.1
  ext_memory_write(GRegisters().addressLatch, GRegisters().byteLatch);
  // 6.2 do operation
  switch (GRegisters().opCode) {
  case OPCODE_ASL_AB_X:
    cpu_asl();
    break;
  case OPCODE_LSR_AB_X:
    cpu_lsr();
    break;
  case OPCODE_ROL_AB_X:
    cpu_rol();
    break;
  case OPCODE_ROR_AB_X:
    cpu_ror();
    break;
  case OPCODE_INC_AB_X:
    cpu_inc();
    break;
  case OPCODE_DEC_AB_X:
    cpu_dec();
    break;
  case OPCODE_SLO_AB_X:
  case OPCODE_SLO_AB_Y:
    cpu_slo();
    break;
  case OPCODE_LSE_AB_X:
  case OPCODE_LSE_AB_Y:
    cpu_lse();
    break;
  case OPCODE_RLA_AB_X:
  case OPCODE_RLA_AB_Y:
    cpu_rla();
    break;
  case OPCODE_RRA_AB_X:
  case OPCODE_RRA_AB_Y:
    cpu_rra();
    break;
  case OPCODE_ISC_AB_X:
  case OPCODE_ISC_AB_Y:
    cpu_isc();
    break;
  case OPCODE_DCP_AB_X:
  case OPCODE_DCP_AB_Y:
    cpu_dcp();
    break;
  case OPCODE_TAS_AB_Y: {
    GRegisters().stack = GRegisters().a & GRegisters().x;
    GRegisters().byteLatch = GRegisters().stack & (GRegisters().addressLatch >> 8);
  } break;
  case OPCODE_AHX_AB_Y: {
    GRegisters().byteLatch = GRegisters().a & GRegisters().x & (GRegisters().addressLatch >> 8);
    break;
  }
  }
  // 7
  MLOG(" W:$%04X <= $%02X", GRegisters().addressLatch, GRegisters().byteLatch)
  ext_memory_write(GRegisters().addressLatch, GRegisters().byteLatch);
}

static inline void memory_rmw_absolute_indexed_x() { memory_rmw_absolute_indexed(GRegisters().x); }

static inline void memory_rmw_absolute_indexed_y() { memory_rmw_absolute_indexed(GRegisters().y); }
/*
#  address R / W description
-- - ------ - -- - ------------------------------------------
1    PC     R  fetch opcode, increment PC
2    PC     R  fetch low byte of address, increment PC
3    PC     R  fetch high byte of address, increment PC
4  address  R  read from effective address
5  address  W  write the value back to effective address,
and do the operation on it
6  address  W  write the new value to effective address
*/

static inline void memory_rwm_absolute()
{
  // 2
  uint pcl = ext_memory_read(GRegisters().pc++);
  // 3
  uint pch = ext_memory_read(GRegisters().pc++) << 8;
  GRegisters().addressLatch = pch | pcl;
  // 4
  GRegisters().byteLatch = ext_memory_read(GRegisters().addressLatch);
  MLOG(" R/W:$%04X <- $%02X", GRegisters().addressLatch, GRegisters().byteLatch);
  // 5.1
  ext_memory_write(GRegisters().addressLatch, GRegisters().byteLatch);
  // 5.1 Do the operation
  switch (GRegisters().opCode) {
    using namespace mnes::opcodes;
  case OPCODE_ASL_AB:
    cpu_asl();
    break;
  case OPCODE_LSR_AB:
    cpu_lsr();
    break;
  case OPCODE_ROL_AB:
    cpu_rol();
    break;
  case OPCODE_ROR_AB:
    cpu_ror();
    break;
  case OPCODE_INC_AB:
    cpu_inc();
    break;
  case OPCODE_DEC_AB:
    cpu_dec();
    break;
  case OPCODE_SLO_AB:
    cpu_slo();
    break;
  case OPCODE_LSE_AB:
    cpu_lse();
    break;
  case OPCODE_RLA_AB:
    cpu_rla();
    break;
  case OPCODE_RRA_AB:
    cpu_rra();
    break;
  case OPCODE_ISC_AB:
    cpu_isc();
    break;
  case OPCODE_DCP_AB:
    cpu_dcp();
    break;
  }
  // 6
  ext_memory_write(GRegisters().addressLatch, GRegisters().byteLatch);
  MLOG(" W:$%04X <= $%02X", GRegisters().addressLatch, GRegisters().byteLatch);
}

/*
 #  address R/W description
--- ------- --- ------------------------------------------
1    PC     R  fetch opcode, increment PC
2    PC     R  fetch address, increment PC
3  address  R  read from effective address
4  address  W  write the value back to effective address,
                and do the operation on it
5  address  W  write the new value to effective address
*/

static inline void memory_rwm_zero_page()
{
  // 2
  GRegisters().addressLatch = ext_memory_read(GRegisters().pc++);
  // 3
  GRegisters().byteLatch = ext_memory_read(GRegisters().addressLatch);
  // 4.1
  MLOG(" $%02X <- $%02X", GRegisters().addressLatch, GRegisters().byteLatch);
  ext_memory_write(GRegisters().addressLatch, GRegisters().byteLatch);
  // 4.2 Do the operation
  switch (GRegisters().opCode) {
    using namespace mnes::opcodes;
  case OPCODE_ASL_ZP:
    cpu_asl();
    break;
  case OPCODE_LSR_ZP:
    cpu_lsr();
    break;
  case OPCODE_ROL_ZP:
    cpu_rol();
    break;
  case OPCODE_ROR_ZP:
    cpu_ror();
    break;
  case OPCODE_INC_ZP:
    cpu_inc();
    break;
  case OPCODE_DEC_ZP:
    cpu_dec();
    break;
  case OPCODE_SLO_ZP:
    cpu_slo();
    break;
  case OPCODE_LSE_ZP:
    cpu_lse();
    break;
  case OPCODE_RLA_ZP:
    cpu_rla();
    break;
  case OPCODE_RRA_ZP:
    cpu_rra();
    break;
  case OPCODE_ISC_ZP:
    cpu_isc();
    break;
  case OPCODE_DCP_ZP:
    cpu_dcp();
    break;
  }
  // 5
  MLOG(" W:$%02X <= $%02X", GRegisters().addressLatch, GRegisters().byteLatch);
  ext_memory_write(GRegisters().addressLatch, GRegisters().byteLatch);
}

/*
 #   address  R/W description
--- --------- --- ---------------------------------------------
1     PC      R  fetch opcode, increment PC
2     PC      R  fetch address, increment PC
3   address   R  read from address, add index register X to it
4  address+X* R  read from effective address
5  address+X* W  write the value back to effective address,
                    and do the operation on it
6  address+X* W  write the new value to effective address

Note: * The high byte of the effective address is always zero,
        i.e. page boundary crossings are not handled.
*/

static inline void memory_rwm_zero_page_indexed_x()
{
  // 2
  uint address = ext_memory_read(GRegisters().pc++);
  // 3.1 - read from address
  MLOG(" $%02X, X[$%02X]", address, GRegisters().x)
  // this is wrong should read from address and throw away
  ext_memory_read(address);
  // 3.2 - add index register x to address w/ zero page
  const uint effectiveAddress = TO_ZERO_PAGE(address + GRegisters().x);
  // 4
  GRegisters().byteLatch = ext_memory_read(effectiveAddress);
  MLOG(" R/W:$%02X <- $%02X", effectiveAddress, GRegisters().byteLatch);
  // 5.1
  ext_memory_write(effectiveAddress, GRegisters().byteLatch);
  // 5.2 Do Operation
  switch (GRegisters().opCode) {
    using namespace mnes::opcodes;
  case OPCODE_ASL_ZP_X:
    cpu_asl();
    break;
  case OPCODE_LSR_ZP_X:
    cpu_lsr();
    break;
  case OPCODE_ROL_ZP_X:
    cpu_rol();
    break;
  case OPCODE_ROR_ZP_X:
    cpu_ror();
    break;
  case OPCODE_INC_ZP_X:
    cpu_inc();
    break;
  case OPCODE_DEC_ZP_X:
    cpu_dec();
    break;
  case OPCODE_SLO_ZP_X:
    cpu_slo();
    break;
  case OPCODE_LSE_ZP_X:
    cpu_lse();
    break;
  case OPCODE_RLA_ZP_X:
    cpu_rla();
    break;
  case OPCODE_RRA_ZP_X:
    cpu_rra();
    break;
  case OPCODE_ISC_ZP_X:
    cpu_isc();
    break;
  case OPCODE_DCP_ZP_X:
    cpu_dcp();
    break;
  }
  // 6
  MLOG(" W:$%02X <= $%02X", effectiveAddress, GRegisters().byteLatch);
  ext_memory_write(effectiveAddress, GRegisters().byteLatch);
}

/*
* SLO, SRE, RLA, RRA, ISB, DCP
*
#    address   R / W description
-- - ---------- - -- - ------------------------------------------
1      PC       R  fetch opcode, increment PC
2      PC       R  fetch pointer address, increment PC
3    pointer    R  read from the address, add X to it
4   pointer + X   R  fetch effective address low
5  pointer + X + 1  R  fetch effective address high
6    address    R  read from effective address
7    address    W  write the value back to effective address,
and do the operation on it
8    address    W  write the new value to effective address

Note : The effective address is always fetched from zero page,
i.e.the zero page boundary crossing is not handled.
*/

static inline void memory_rwm_indexed_indirect()
{
  // 2
  uint pointer = ext_memory_read(GRegisters().pc++);
  MLOG(" ($%02X, X[$%02X])", pointer, GRegisters().x)
  // 3.1 - read from pointer address, result thrown away
  ext_memory_read(pointer);
  // 3.2 - add X to pointer address
  pointer += GRegisters().x;
  // 4
  uint pcl = ext_memory_read(TO_ZERO_PAGE(pointer));
  // 5
  uint pch = ext_memory_read(TO_ZERO_PAGE(pointer + 1));
  GRegisters().addressLatch = pcl | (pch << 8);
  // 6
  GRegisters().byteLatch = ext_memory_read(GRegisters().addressLatch);
  MLOG(" R/W:$%04X <- $%02X", GRegisters().addressLatch, GRegisters().byteLatch);
  // 7.1
  ext_memory_write(GRegisters().addressLatch, GRegisters().byteLatch);
  // 7.2
  switch (GRegisters().opCode) {
    using namespace mnes::opcodes;
  case OPCODE_SLO_IN_X:
    cpu_slo();
    break;
  case OPCODE_LSE_IN_X:
    cpu_lse();
    break;
  case OPCODE_RLA_IN_X:
    cpu_rla();
    break;
  case OPCODE_RRA_IN_X:
    cpu_rra();
    break;
  case OPCODE_ISC_IN_X:
    cpu_isc();
    break;
  case OPCODE_DCP_IN_X:
    cpu_dcp();
    break;
  case OPCODE_LAX_IN_X:
    cpu_lax();
    break;
  }
  // 8
  MLOG(" W:$%04X <= $%02X", GRegisters().addressLatch, GRegisters().byteLatch);
  ext_memory_write(GRegisters().addressLatch, GRegisters().byteLatch);
}

/*
        #    address   R/W description
       --- ----------- --- ------------------------------------------
        1      PC       R  fetch opcode, increment PC
        2      PC       R  fetch pointer address, increment PC
        3    pointer    R  fetch effective address low
        4   pointer+1   R  fetch effective address high,
                           add Y to low byte of effective address
        5   address+Y*  R  read from effective address,
                           fix high byte of effective address
        6   address+Y   R  read from effective address
        7   address+Y   W  write the value back to effective address,
                           and do the operation on it
        8   address+Y   W  write the new value to effective address

       Notes: The effective address is always fetched from zero page,
              i.e. the zero page boundary crossing is not handled.

              * The high byte of the effective address may be invalid
                at this time, i.e. it may be smaller by $100.
*/

static inline void memory_rwm_indirect_indexed()
{
  // 2
  uint pointer = ext_memory_read(GRegisters().pc++);
  MLOG(" ($%02X), Y[%02X]", pointer, GRegisters().y);
  // 3
  uint pcl = ext_memory_read(pointer);
  // 4
  uint pch = ext_memory_read(TO_ZERO_PAGE(pointer + 1)) << 8;
  pcl += GRegisters().y;
  // 5.1 read from effective address, may be invalid
  MLOG(" DR:$%04X", pch | TO_ZERO_PAGE(pcl))
  ext_memory_read(TO_ZERO_PAGE(pcl) | pch);
  // 5.2 Fix PCH
  GRegisters().addressLatch = (pch + pcl) & 0xFFFF;
  // 6
  GRegisters().byteLatch = ext_memory_read(GRegisters().addressLatch);
  MLOG(" R/W:$%04X <- $%02X", GRegisters().addressLatch, GRegisters().byteLatch)
  // 7.1
  ext_memory_write(GRegisters().addressLatch, GRegisters().byteLatch);
  // 7.2 - Do Operation
  switch (GRegisters().opCode) {
    using namespace mnes::opcodes;
  case OPCODE_SLO_IN_Y:
    cpu_slo();
    break;
  case OPCODE_LSE_IN_Y:
    cpu_lse();
    break;
  case OPCODE_RLA_IN_Y:
    cpu_rla();
    break;
  case OPCODE_RRA_IN_Y:
    cpu_rra();
    break;
  case OPCODE_ISC_IN_Y:
    cpu_isc();
    break;
  case OPCODE_DCP_IN_Y:
    cpu_dcp();
    break;
  case OPCODE_AHX_IN_Y:
    GRegisters().byteLatch = GRegisters().a & GRegisters().x & (GRegisters().addressLatch >> 8);
    break;
  case OPCODE_LAX_IN_Y:
    cpu_lax();
    break;
  }
  // 8
  MLOG(" W:$%04X <= $%02X", GRegisters().addressLatch, GRegisters().byteLatch)
  ext_memory_write(GRegisters().addressLatch, GRegisters().byteLatch);
}
