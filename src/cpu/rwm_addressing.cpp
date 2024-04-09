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

namespace {
void mnes_::memory::rmw_absolute_indexed(const uint32_t &indexRegister)
{
  using namespace mnes::opcodes;
  // 2
  uint32_t pcl = ext::read(cpureg.pc++);
  // 3
  uint32_t pch = ext::read(cpureg.pc++) << 8;
  MLOG("operand[${:04X}] + index[${:02X}]", pcl | pch, indexRegister);
  pcl += indexRegister;
  // 4
  MLOG("fake read address:${:04X}", pch | (pcl & 0xFF));
  ext::read(pch | (pcl & 0xFF));
  cpureg.addressLatch = (pch + pcl) & 0xFFFF;
  if (cpureg.opCode == OPCODE_LAS_AB_Y) {
    cpureg.byteLatch = cpureg.stack & (cpureg.addressLatch >> 8);
    cpureg.a = cpureg.x = cpureg.stack = cpureg.byteLatch;
    set_nz(cpureg.a);
    return;// odd behavior skips the rest
  }
  // 5
  cpureg.byteLatch = ext::read(cpureg.addressLatch);
  MLOG("effective_address:${:04X} <- data:${:02X}", cpureg.addressLatch, cpureg.byteLatch);
  // 6.1
  ext::write(cpureg.addressLatch, cpureg.byteLatch);
  // 6.2 do operation
  switch (cpureg.opCode) {
  case OPCODE_ASL_AB_X:
    asl();
    break;
  case OPCODE_LSR_AB_X:
    lsr();
    break;
  case OPCODE_ROL_AB_X:
    rol();
    break;
  case OPCODE_ROR_AB_X:
    ror();
    break;
  case OPCODE_INC_AB_X:
    inc();
    break;
  case OPCODE_DEC_AB_X:
    dec();
    break;
  case OPCODE_SLO_AB_X:
  case OPCODE_SLO_AB_Y:
    slo();
    break;
  case OPCODE_LSE_AB_X:
  case OPCODE_LSE_AB_Y:
    lse();
    break;
  case OPCODE_RLA_AB_X:
  case OPCODE_RLA_AB_Y:
    rla();
    break;
  case OPCODE_RRA_AB_X:
  case OPCODE_RRA_AB_Y:
    rra();
    break;
  case OPCODE_ISC_AB_X:
  case OPCODE_ISC_AB_Y:
    isc();
    break;
  case OPCODE_DCP_AB_X:
  case OPCODE_DCP_AB_Y:
    dcp();
    break;
  case OPCODE_TAS_AB_Y: {
    cpureg.stack = cpureg.a & cpureg.x;
    cpureg.byteLatch = cpureg.stack & (cpureg.addressLatch >> 8);
  } break;
  case OPCODE_AHX_AB_Y: {
    cpureg.byteLatch = cpureg.a & cpureg.x & (cpureg.addressLatch >> 8);
    break;
  }
  }
  // 7
  MLOG("write_address:${:04X} <= data: ${:02X}", cpureg.addressLatch, cpureg.byteLatch);
  ext::write(cpureg.addressLatch, cpureg.byteLatch);
}

void mnes_::memory::rmw_absolute_indexed_x() { rmw_absolute_indexed(cpureg.x); }

void mnes_::memory::rmw_absolute_indexed_y() { rmw_absolute_indexed(cpureg.y); }
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

void mnes_::memory::rwm_absolute()
{
  // 2
  uint32_t pcl = ext::read(cpureg.pc++);
  // 3
  uint32_t pch = ext::read(cpureg.pc++) << 8;
  cpureg.addressLatch = pch | pcl;
  // 4
  cpureg.byteLatch = ext::read(cpureg.addressLatch);
  MLOG(" R/W:${:04X} <- ${:02X}", cpureg.addressLatch, cpureg.byteLatch);
  // 5.1
  ext::write(cpureg.addressLatch, cpureg.byteLatch);
  // 5.1 Do the operation
  switch (cpureg.opCode) {
    using namespace mnes::opcodes;
  case OPCODE_ASL_AB:
    asl();
    break;
  case OPCODE_LSR_AB:
    lsr();
    break;
  case OPCODE_ROL_AB:
    rol();
    break;
  case OPCODE_ROR_AB:
    ror();
    break;
  case OPCODE_INC_AB:
    inc();
    break;
  case OPCODE_DEC_AB:
    dec();
    break;
  case OPCODE_SLO_AB:
    slo();
    break;
  case OPCODE_LSE_AB:
    lse();
    break;
  case OPCODE_RLA_AB:
    rla();
    break;
  case OPCODE_RRA_AB:
    rra();
    break;
  case OPCODE_ISC_AB:
    isc();
    break;
  case OPCODE_DCP_AB:
    dcp();
    break;
  }
  // 6
  ext::write(cpureg.addressLatch, cpureg.byteLatch);
  MLOG(" W:${:04X} <= ${:02X}", cpureg.addressLatch, cpureg.byteLatch);
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

void mnes_::memory::rwm_zero_page()
{
  // 2
  cpureg.addressLatch = ext::read(cpureg.pc++);
  // 3
  cpureg.byteLatch = ext::read(cpureg.addressLatch);
  // 4.1
  MLOG(" ${:02X} <- ${:02X}", cpureg.addressLatch, cpureg.byteLatch);
  ext::write(cpureg.addressLatch, cpureg.byteLatch);
  // 4.2 Do the operation
  switch (cpureg.opCode) {
    using namespace mnes::opcodes;
  case OPCODE_ASL_ZP:
    asl();
    break;
  case OPCODE_LSR_ZP:
    lsr();
    break;
  case OPCODE_ROL_ZP:
    rol();
    break;
  case OPCODE_ROR_ZP:
    ror();
    break;
  case OPCODE_INC_ZP:
    inc();
    break;
  case OPCODE_DEC_ZP:
    dec();
    break;
  case OPCODE_SLO_ZP:
    slo();
    break;
  case OPCODE_LSE_ZP:
    lse();
    break;
  case OPCODE_RLA_ZP:
    rla();
    break;
  case OPCODE_RRA_ZP:
    rra();
    break;
  case OPCODE_ISC_ZP:
    isc();
    break;
  case OPCODE_DCP_ZP:
    dcp();
    break;
  }
  // 5
  MLOG(" W:${:02X} <= ${:02X}", cpureg.addressLatch, cpureg.byteLatch);
  ext::write(cpureg.addressLatch, cpureg.byteLatch);
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

void mnes_::memory::rwm_zero_page_indexed_x()
{
  // 2
  uint32_t address = ext::read(cpureg.pc++);
  // 3.1 - read from address
  MLOG("fake read address[${:02X}], then + X[${:02X}]", address, cpureg.x);
  // this is wrong should read from address and throw away
  ext::read(address);
  // 3.2 - add index register x to address w/ zero page
  const uint32_t effectiveAddress = TO_ZERO_PAGE(address + cpureg.x);
  // 4
  cpureg.byteLatch = ext::read(effectiveAddress);
  MLOG("read_effective_address:${:02X} <- data: ${:02X}", effectiveAddress, cpureg.byteLatch);
  // 5.1
  ext::write(effectiveAddress, cpureg.byteLatch);
  // 5.2 Do Operation
  switch (cpureg.opCode) {
    using namespace mnes::opcodes;
  case OPCODE_ASL_ZP_X:
    asl();
    break;
  case OPCODE_LSR_ZP_X:
    lsr();
    break;
  case OPCODE_ROL_ZP_X:
    rol();
    break;
  case OPCODE_ROR_ZP_X:
    ror();
    break;
  case OPCODE_INC_ZP_X:
    inc();
    break;
  case OPCODE_DEC_ZP_X:
    dec();
    break;
  case OPCODE_SLO_ZP_X:
    slo();
    break;
  case OPCODE_LSE_ZP_X:
    lse();
    break;
  case OPCODE_RLA_ZP_X:
    rla();
    break;
  case OPCODE_RRA_ZP_X:
    rra();
    break;
  case OPCODE_ISC_ZP_X:
    isc();
    break;
  case OPCODE_DCP_ZP_X:
    dcp();
    break;
  }
  // 6
  MLOG(" W:${:02X} <= ${:02X}", effectiveAddress, cpureg.byteLatch);
  ext::write(effectiveAddress, cpureg.byteLatch);
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

void mnes_::memory::rwm_indexed_indirect()
{
  // 2
  uint32_t pointer = ext::read(cpureg.pc++);
  // 3.1 - read from pointer address, result thrown away
  ext::read(pointer);
  MLOG("pointer[${:02X}] + X[${:02X}])", pointer, cpureg.x);
  // 3.2 - add X to pointer address
  pointer += cpureg.x;
  
  // 4
  uint32_t pcl = ext::read(TO_ZERO_PAGE(pointer));
  // 5
  uint32_t pch = ext::read(TO_ZERO_PAGE(pointer + 1));
  cpureg.addressLatch = pcl | (pch << 8);
  // 6
  cpureg.byteLatch = ext::read(cpureg.addressLatch);
  MLOG("rw_effective_address: ${:04X} <- data: ${:02X}", cpureg.addressLatch, cpureg.byteLatch);
  // 7.1
  ext::write(cpureg.addressLatch, cpureg.byteLatch);
  // 7.2
  switch (cpureg.opCode) {
    using namespace mnes::opcodes;
  case OPCODE_SLO_IN_X:
    slo();
    break;
  case OPCODE_LSE_IN_X:
    lse();
    break;
  case OPCODE_RLA_IN_X:
    rla();
    break;
  case OPCODE_RRA_IN_X:
    rra();
    break;
  case OPCODE_ISC_IN_X:
    isc();
    break;
  case OPCODE_DCP_IN_X:
    dcp();
    break;
  case OPCODE_LAX_IN_X:
    lax();
    break;
  }
  // 8
  MLOG(" W:${:04X} <= ${:02X}", cpureg.addressLatch, cpureg.byteLatch);
  ext::write(cpureg.addressLatch, cpureg.byteLatch);
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

void mnes_::memory::rwm_indirect_indexed()
{
  // 2
  uint32_t pointer = ext::read(cpureg.pc++);
  MLOG("indirect_pointer[${:02X}] + Y[{:02X}]", pointer, cpureg.y);
  // 3
  uint32_t pcl = ext::read(pointer);
  // 4
  uint32_t pch = ext::read(TO_ZERO_PAGE(pointer + 1)) << 8;
  pcl += cpureg.y;
  // 5.1 read from effective address, may be invalid
  MLOG("fake_read_effective_address:${:04X} data: throwaway", pch | TO_ZERO_PAGE(pcl));
  ext::read(TO_ZERO_PAGE(pcl) | pch);
  // 5.2 Fix PCH
  //TODO: do we really allow pcl to overflow over 0x100 ?
  cpureg.addressLatch = (pch + pcl) & 0xFFFF;
  // 6
  cpureg.byteLatch = ext::read(cpureg.addressLatch);
  MLOG("rw_effective_address: ${:04X} <- data: ${:02X}", cpureg.addressLatch, cpureg.byteLatch);
  // 7.1
  ext::write(cpureg.addressLatch, cpureg.byteLatch);
  // 7.2 - Do Operation
  switch (cpureg.opCode) {
    using namespace mnes::opcodes;
  case OPCODE_SLO_IN_Y:
    slo();
    break;
  case OPCODE_LSE_IN_Y:
    lse();
    break;
  case OPCODE_RLA_IN_Y:
    rla();
    break;
  case OPCODE_RRA_IN_Y:
    rra();
    break;
  case OPCODE_ISC_IN_Y:
    isc();
    break;
  case OPCODE_DCP_IN_Y:
    dcp();
    break;
  case OPCODE_AHX_IN_Y:
    cpureg.byteLatch = cpureg.a & cpureg.x & (cpureg.addressLatch >> 8);
    break;
  case OPCODE_LAX_IN_Y:
    lax();
    break;
  }
  // 8
  MLOG("w_effective_address: ${:04X} <= data: ${:02X}", cpureg.addressLatch, cpureg.byteLatch);
  ext::write(cpureg.addressLatch, cpureg.byteLatch);
}
}