/*
Write instructions(STA, STX, STY, SAX)

#  address R / W description
-- - ------ - -- - ------------------------------------------
1    PC     R  fetch opcode, increment PC
2    PC     R  fetch low byte of address, increment PC
3    PC     R  fetch high byte of address, increment PC
4  address  W  write register to effective address
*/

namespace {

void mnes_::memory::w_absolute()
{
  // 2
  uint32_t pcl = ext::read(cpureg.pc++);
  // 3
  uint32_t pch = ext::read(cpureg.pc++) << 8;
  // 4.1 Do Operation
  switch (cpureg.opCode) {
    using namespace mnes::opcodes;
  case OPCODE_STA_AB:
    sta();
    break;
  case OPCODE_STX_AB:
    stx();
    break;
  case OPCODE_STY_AB:
    sty();
    break;
  }
  // 4.2
  MLOG(" ${:04X} <= ${:02X}", pcl | pch, cpureg.byteLatch);
  ext::write(pcl | pch, cpureg.byteLatch);
}

/*
         Write instructions (STA, STX, STY, SAX)

                #  address R/W description
           --- ------- --- ------------------------------------------
                1    PC     R  fetch opcode, increment PC
                2    PC     R  fetch address, increment PC
                3  address  W  write register to effective address
*/

void mnes_::memory::w_zero_page()
{
  // 2
  uint32_t address = ext::read(cpureg.pc++);
  // 3.1
  switch (cpureg.opCode) {
    using namespace mnes::opcodes;
  case OPCODE_STA_ZP:
    sta();
    break;
  case OPCODE_STX_ZP:
    stx();
    break;
  case OPCODE_STY_ZP:
    sty();
    break;
  case OPCODE_SAX_ZP:
    sax();
    break;
  }
  // 3.2
  MLOG("address: ${:02X} <= data: ${:02X}", address, cpureg.byteLatch);

  ext::write(address, cpureg.byteLatch);
}

/*
         Write instructions (STA, STX, STY, SAX)

                #   address  R/W description
           --- --------- --- -------------------------------------------
                1     PC      R  fetch opcode, increment PC
                2     PC      R  fetch address, increment PC
                3   address   R  read from address, add index register to it
                4  address+I* W  write to effective address

           Notes: I denotes either index register (X or Y).

                          * The high byte of the effective address is always zero,
                                i.e. page boundary crossings are not handled.
*/

void mnes_::memory::w_zero_page_indexed(const uint32_t &indexRegister)
{
  // 2
  uint32_t address = ext::read(cpureg.pc++);
  // 3.1 read from address
  ext::read(address);
  MLOG(" ${:02X}, I[${:02X}]", address, indexRegister);
  // 3.2 add index register to address
  address = TO_ZERO_PAGE(address + indexRegister);
  // 4.1 Do Operation
  switch (cpureg.opCode) {
    using namespace mnes::opcodes;
  case OPCODE_STA_ZP_X:
    sta();
    break;
  case OPCODE_STY_ZP_X:
    sty();
    break;
  case OPCODE_STX_ZP_Y:
    stx();
    break;
  case OPCODE_SAX_ZP_Y:
    sax();
    break;
  }
  // 4.2
  MLOG("address:${:02X} <= data:${:02X}", address, cpureg.byteLatch);
  ext::write(address, cpureg.byteLatch);
}

void mnes_::memory::w_zero_page_indexed_x() { w_zero_page_indexed(cpureg.x); }

void mnes_::memory::w_zero_page_indexed_y() { w_zero_page_indexed(cpureg.y); }

/*
         Write instructions (STA, STX, STY, SHA, SHX, SHY)

                #   address  R/W description
           --- --------- --- ------------------------------------------
                1     PC      R  fetch opcode, increment PC
                2     PC      R  fetch low byte of address, increment PC
                3     PC      R  fetch high byte of address,
                                                 add index register to low address byte,
                                                 increment PC
                4  address+I* R  read from effective address,
                                                 fix the high byte of effective address
                5  address+I  W  write to effective address

           Notes: I denotes either index register (X or Y).

                          * The high byte of the effective address may be invalid
                                at this time, i.e. it may be smaller by $100. Because
                                the processor cannot undo a write to an invalid
                                address, it always reads from the address first.
*/

void mnes_::memory::w_absolute_indexed(const uint32_t &indexRegister)
{
  // 2
  uint32_t pcl = ext::read(cpureg.pc++);
  // 3.1 fetch high byte of address
  uint32_t pch = ext::read(cpureg.pc++) << 8;
  MLOG(" ${:04X}, I[${:02X}]", pcl | pch, indexRegister);
  // 3.2 add index register to low address byte
  pcl += indexRegister;
  // 3.3 increase PC (already done)
  // 4.1 read from effective address
  ext::read(TO_ZERO_PAGE(pcl) | pch);
  // 4.2 fix high byte
  uint32_t address = (pcl + pch) & 0xFFFF;
  // 5.1 Do Operation
  switch (cpureg.opCode) {
    using namespace mnes::opcodes;
  case OPCODE_STA_AB_X:
  case OPCODE_STA_AB_Y:
    sta();
    break;
  case OPCODE_SHY_AB_X:
    cpureg.byteLatch = (cpureg.y & ((address >> 8) + 1)) & 0xFF;
    if ((cpureg.x + pcl) > 0xFF) { return; }
    break;
  case OPCODE_SHX_AB_Y:
    cpureg.byteLatch = (cpureg.x & ((address >> 8) + 1)) & 0xFF;
    if ((cpureg.y + pcl) > 0xFF) { return; }
    break;
  }
  // 5.2
  MLOG("address:${:04X} <= data: ${:02X}", address, cpureg.byteLatch);
  ext::write(address, cpureg.byteLatch);
}

void mnes_::memory::w_absolute_indexed_x() { w_absolute_indexed(cpureg.x); }

void mnes_::memory::w_absolute_indexed_y() { w_absolute_indexed(cpureg.y); }

/*
         Write instructions (STA, SAX)

                #    address   R/W description
           --- ----------- --- ------------------------------------------
                1      PC       R  fetch opcode, increment PC
                2      PC       R  fetch pointer address, increment PC
                3    pointer    R  read from the address, add X to it
                4   pointer+X   R  fetch effective address low
                5  pointer+X+1  R  fetch effective address high
                6    address    W  write to effective address

           Note: The effective address is always fetched from zero page,
                         i.e. the zero page boundary crossing is not handled.
*/

void mnes_::memory::w_indexed_indirect()
{
  // 2
  uint32_t pointer = ext::read(cpureg.pc++);
  MLOG("(${:02X}, X[${:02X}])", pointer, cpureg.x);
  // 3.1 read from pointer address, throw away data
  ext::read(pointer);
  // 3.2 add X to pointer address
  pointer += cpureg.x;
  // 4
  uint32_t pcl = ext::read(TO_ZERO_PAGE(pointer));
  // 5
  uint32_t pch = ext::read(TO_ZERO_PAGE(pointer + 1)) << 8;
  // 6.1 Do Operation
  switch (cpureg.opCode) {
    using namespace mnes::opcodes;
  case OPCODE_STA_IN_X:
    sta();
    break;
  }
  // 6.2
  MLOG("address:${:04X} <= data:${:02X}", pcl | pch, cpureg.byteLatch);
  ext::write(pcl | pch, cpureg.byteLatch);
}
/*
         Write instructions (STA, SHA)

                #    address   R/W description
           --- ----------- --- ------------------------------------------
                1      PC       R  fetch opcode, increment PC
                2      PC       R  fetch pointer address, increment PC
                3    pointer    R  fetch effective address low
                4   pointer+1   R  fetch effective address high,
                                                   add Y to low byte of effective address
                5   address+Y*  R  read from effective address,
                                                   fix high byte of effective address
                6   address+Y   W  write to effective address

           Notes: The effective address is always fetched from zero page,
                          i.e. the zero page boundary crossing is not handled.

                          * The high byte of the effective address may be invalid
                                at this time, i.e. it may be smaller by $100.
*/

void mnes_::memory::w_indirect_indexed()
{
  // 2
  uint32_t pointer = ext::read(cpureg.pc++);
  MLOG(" (${:02X}), Y[{:02X}]", pointer, cpureg.y);
  // 3
  uint32_t pcl = ext::read(pointer);
  // 4.1 fetch high byte of effective address
  uint32_t pch = ext::read(TO_ZERO_PAGE(pointer + 1)) << 8;
  // 4.2 add Y to low byte of effective address
  pcl += cpureg.y;
  // 5.1 read from effective address could be invalid
  ext::read(TO_ZERO_PAGE(pcl) | pch);
  // 5.2 fix high byte of effective address
  uint32_t address = (pcl + pch) & 0xFFFF;
  // 6.1 Do Operation
  switch (cpureg.opCode) {
    using namespace mnes::opcodes;
  case OPCODE_STA_IN_Y:
    sta();
    break;
  }
  // 6.2
  MLOG(" A:${:04X} <= ${:02X}", address, cpureg.byteLatch);
  ext::write(address, cpureg.byteLatch);
}

}