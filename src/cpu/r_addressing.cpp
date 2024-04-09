namespace {
void mnes_::memory::r_immediate()
{
  // 2.1
  cpureg.byteLatch = ext::read(cpureg.pc++);

  MLOG("#${:02X}", cpureg.byteLatch);

  // 2.2 Do Operation
  switch (cpureg.opCode) {
    using namespace mnes::opcodes;
  case OPCODE_ADC_OP:
    adc();
    break;
  case OPCODE_AND_OP:
    and_();
    break;
  case OPCODE_CMP_OP:
    cmp();
    break;
  case OPCODE_CPX_OP:
    cpx();
    break;
  case OPCODE_CPY_OP:
    cpy();
    break;
  case OPCODE_EOR_OP:
    eor();
    break;
  case OPCODE_LDA_OP:
    lda();
    break;
  case OPCODE_LDX_OP:
    ldx();
    break;
  case OPCODE_LDY_OP:
    ldy();
    break;
  case OPCODE_ORA_OP:
    ora();
    break;
  case OPCODE_SBC_OP:
    sbc();
    break;
  case OPCODE_SBC_IMM_EB:
    cpureg.byteLatch = 255 - cpureg.byteLatch;
    sbc();
    break;
  case OPCODE_ARR_IMM:
    arr();
    break;
  case OPCODE_NOP_IMM:
    break;
  case OPCODE_ANC_IMM_0B:
  case OPCODE_ANC_IMM_2B:
    and_();
    set_carry(IF_SIGN());
    break;
  case OPCODE_ALR_IMM:
    and_();
    internals::lsr(cpureg.a);
    break;
  case OPCODE_LAX_IMM:
    lax();
    break;
  case OPCODE_AXS_IMM:
    /*SAX***
        SAX ANDs the contents of the Aand X registers(leaving the contents of A
            intact), subtracts an immediate value, and then stores the result in X.
        ... A few points might be made about the action of subtracting an immediate
        value.It actually works just like the CMP instruction, except that CMP
        does not store the result of the subtraction it performs in any register.
        This subtract operation is not affected by the state of the Carry flag,
        though it does affect the Carry flag.It does not affect the Overflow
        flag.

        One supported mode :

    SAX #ab; CB ab; No.Cycles = 2

        Example:

    SAX #$5A; CB 5A

        Equivalent instructions :

    STA $02
        TXA
        AND $02
        SEC
        SBC #$5A
        TAX
        LDA $02
        */
    axs();
    break;
  case OPCODE_XAA_IMM:
    xaa();
    break;
  }
}


/*
     Read instructions (LDA, LDX, LDY, EOR, AND, ORA, ADC, SBC, CMP, BIT,
                        LAX, NOP)

        #  address R/W description
       --- ------- --- ------------------------------------------
        1    PC     R  fetch opcode, increment PC
        2    PC     R  fetch low byte of address, increment PC
        3    PC     R  fetch high byte of address, increment PC
        4  address  R  read from effective address
*/
void mnes_::memory::r_absolute()
{
  // 2
  uint32_t pcl = ext::read(cpureg.pc++);
  // 3
  uint32_t pch = ext::read(cpureg.pc++);
  // 4.1
  cpureg.byteLatch = ext::read(pcl | (pch << 8));
  MLOG("${:04X} <- ${:02X}", pcl | (pch << 8), cpureg.byteLatch);
  // 4.2 - Do Operation
  switch (cpureg.opCode) {
    using namespace mnes::opcodes;
  case OPCODE_LDA_AB:
    lda();
    break;
  case OPCODE_LDX_AB:
    ldx();
    break;
  case OPCODE_LDY_AB:
    ldy();
    break;
  case OPCODE_EOR_AB:
    eor();
    break;
  case OPCODE_AND_AB:
    and_();
    break;
  case OPCODE_ORA_AB:
    ora();
    break;
  case OPCODE_ADC_AB:
    adc();
    break;
  case OPCODE_SBC_AB:
    sbc();
    break;
  case OPCODE_CMP_AB:
    cmp();
    break;
  case OPCODE_BIT_AB:
    bit();
    break;
  case OPCODE_LAX_AB:
    lax();
    break;
  case OPCODE_CPX_AB:
    cpx();
    break;
  case OPCODE_CPY_AB:
    cpy();
    break;
  }
}
/*
     Read instructions (LDA, LDX, LDY, EOR, AND, ORA, ADC, SBC, CMP, BIT,
                        LAX, NOP)

        #  address R/W description
       --- ------- --- ------------------------------------------
        1    PC     R  fetch opcode, increment PC
        2    PC     R  fetch address, increment PC
        3  address  R  read from effective address

*/

void mnes_::memory::r_zero_page()
{
  // 2
  uint32_t address = ext::read(cpureg.pc++);
  // 3.1
  cpureg.byteLatch = ext::read(address);
  MLOG("${:02X} <- ${:02X}", address, cpureg.byteLatch);
  // 3.2
  switch (cpureg.opCode) {
    using namespace mnes::opcodes;
  case OPCODE_LDA_ZP:
    lda();
    break;
  case OPCODE_LDX_ZP:
    ldx();
    break;
  case OPCODE_LDY_ZP:
    ldy();
    break;
  case OPCODE_EOR_ZP:
    eor();
    break;
  case OPCODE_AND_ZP:
    and_();
    break;
  case OPCODE_ORA_ZP:
    ora();
    break;
  case OPCODE_ADC_ZP:
    adc();
    MLOG("A<=${:02X}", cpureg.a);
    break;
  case OPCODE_SBC_ZP:
    sbc();
    break;
  case OPCODE_CMP_ZP:
    cmp();
    break;
  case OPCODE_BIT_ZP:
    bit();
    break;
  case OPCODE_LAX_ZP:
    lax();
    break;
  case OPCODE_CPX_ZP:
    cpx();
    break;
  case OPCODE_CPY_ZP:
    cpy();
    break;
  }
}

/*
     Read instructions (LDA, LDX, LDY, EOR, AND, ORA, ADC, SBC, CMP, BIT,
                        LAX, NOP)

        #   address  R/W description
       --- --------- --- ------------------------------------------
        1     PC      R  fetch opcode, increment PC
        2     PC      R  fetch address, increment PC
        3   address   R  read from address, add index register to it
        4  address+I* R  read from effective address

       Notes: I denotes either index register (X or Y).

              * The high byte of the effective address is always zero,
                i.e. page boundary crossings are not handled.

*/

void mnes_::memory::r_zero_page_indexed(const uint32_t &indexRegister)
{
  // 2
  uint32_t address = ext::read(cpureg.pc++);
  // 3
  ext::read(address);
  // 4.1
  cpureg.byteLatch = ext::read((address + indexRegister) & 0xFF);
  MLOG("${:02X}, I[{:02X}] A:${:02X} <- ${:02X}",
    address,
    indexRegister,
    (address + indexRegister) & 0xFF,
    cpureg.byteLatch);
  // 4.2
  switch (cpureg.opCode) {
    using namespace mnes::opcodes;
  case OPCODE_LDA_ZP_X:
    lda();
    break;
  case OPCODE_LDY_ZP_X:
    ldy();
    break;
  case OPCODE_EOR_ZP_X:
    eor();
    break;
  case OPCODE_AND_ZP_X:
    and_();
    break;
  case OPCODE_ORA_ZP_X:
    ora();
    break;
  case OPCODE_ADC_ZP_X:
    adc();
    break;
  case OPCODE_SBC_ZP_X:
    sbc();
    break;
  case OPCODE_CMP_ZP_X:
    cmp();
    break;
  case OPCODE_LDX_ZP_Y:
    ldx();
    break;
  case OPCODE_LAX_ZP_Y:
    lax();
    break;
  }
}

void mnes_::memory::r_zero_page_indexed_x() { r_zero_page_indexed(cpureg.x); }

void mnes_::memory::r_zero_page_indexed_y() { r_zero_page_indexed(cpureg.y); }

/*
     Read instructions (LDA, LDX, LDY, EOR, AND, ORA, ADC, SBC, CMP, BIT,
                        LAX, LAE, SHS, NOP)

        #   address  R/W description
       --- --------- --- ------------------------------------------
        1     PC      R  fetch opcode, increment PC
        2     PC      R  fetch low byte of address, increment PC
        3     PC      R  fetch high byte of address,
                         add index register to low address byte,
                         increment PC
        4  address+I* R  read from effective address,
                         fix the high byte of effective address
        5+ address+I  R  re-read from effective address

       Notes: I denotes either index register (X or Y).

              * The high byte of the effective address may be invalid
                at this time, i.e. it may be smaller by $100.

              + This cycle will be executed only if the effective address
                was invalid during cycle #4, i.e. page boundary was crossed.
*/

void mnes_::memory::r_absolute_indexed(const uint32_t &indexRegister)
{
  // 2
  uint32_t pcl = ext::read(cpureg.pc++);
  // 3
  uint32_t pch = ext::read(cpureg.pc++) << 8;
  MLOG("${:04X}, I[${:02X}]", pcl | pch, indexRegister);
  pcl += indexRegister;
  // 4
  cpureg.byteLatch = ext::read((pcl & 0xFF) | pch);
  if (pcl > 0xFF) {
    // 5
    cpureg.byteLatch = ext::read((pch + pcl) & 0xFFFF);
    MLOG("AF:${:04X} <- ${:02X}", (pch + pcl) & 0xFFFF, cpureg.byteLatch);
  } else {
    MLOG("A:${:04X} <- ${:02X}", (pcl & 0xFF) | pch, cpureg.byteLatch);
  }
  // Do Operation
  switch (cpureg.opCode) {
    using namespace mnes::opcodes;
  case OPCODE_LDA_AB_X:
  case OPCODE_LDA_AB_Y:
    lda();
    break;
  case OPCODE_LDX_AB_Y:
    ldx();
    break;
  case OPCODE_LDY_AB_X:
    ldy();
    break;
  case OPCODE_EOR_AB_X:
  case OPCODE_EOR_AB_Y:
    eor();
    break;
  case OPCODE_AND_AB_X:
  case OPCODE_AND_AB_Y:
    and_();
    break;
  case OPCODE_ORA_AB_X:
  case OPCODE_ORA_AB_Y:
    ora();
    break;
  case OPCODE_ADC_AB_X:
  case OPCODE_ADC_AB_Y:
    adc();
    break;
  case OPCODE_SBC_AB_X:
  case OPCODE_SBC_AB_Y:
    sbc();
    break;
  case OPCODE_CMP_AB_X:
  case OPCODE_CMP_AB_Y:
    cmp();
    break;
  case OPCODE_LAX_AB_Y:
    lax();
    break;
  }
}

void mnes_::memory::r_absolute_indexed_x() { r_absolute_indexed(cpureg.x); }

void mnes_::memory::r_absolute_indexed_y() { r_absolute_indexed(cpureg.y); }

/*
     Read instructions (LDA, ORA, EOR, AND, ADC, CMP, SBC, LAX)

        #    address   R/W description
       --- ----------- --- ------------------------------------------
        1      PC       R  fetch opcode, increment PC
        2      PC       R  fetch pointer address, increment PC
        3    pointer    R  read from the address, add X to it
        4   pointer+X   R  fetch effective address low
        5  pointer+X+1  R  fetch effective address high
        6    address    R  read from effective address

       Note: The effective address is always fetched from zero page,
             i.e. the zero page boundary crossing is not handled.
*/

void mnes_::memory::r_indexed_indirect()
{
  // 2
  uint32_t pointer = ext::read(cpureg.pc++);
  MLOG("(${:02X}, X[${:02X}])", pointer, cpureg.x);
  // 3
  ext::read(pointer);
  // 4
  uint32_t pcl = ext::read(TO_ZERO_PAGE(pointer + cpureg.x));
  // 5
  uint32_t pch = ext::read(TO_ZERO_PAGE(pointer + cpureg.x + 1)) << 8;
  // 6.1
  cpureg.byteLatch = ext::read(pcl | pch);
  MLOG("address:${:04X} <- data: ${:02X}", pcl | pch, cpureg.byteLatch);
  // 6.2 Do Operation
  switch (cpureg.opCode) {
    using namespace mnes::opcodes;
  case OPCODE_LDA_IN_X:
    lda();
    break;
  case OPCODE_ORA_IN_X:
    ora();
    break;
  case OPCODE_EOR_IN_X:
    eor();
    break;
  case OPCODE_AND_IN_X:
    and_();
    break;
  case OPCODE_ADC_IN_X:
    adc();
    break;
  case OPCODE_CMP_IN_X:
    cmp();
    break;
  case OPCODE_SBC_IN_X:
    sbc();
    break;
  case OPCODE_LAX_IN_X:
    lax();
    break;
  }
}

/*
     Read instructions (LDA, EOR, AND, ORA, ADC, SBC, CMP)

        #    address   R/W description
       --- ----------- --- ------------------------------------------
        1      PC       R  fetch opcode, increment PC
        2      PC       R  fetch pointer address, increment PC
        3    pointer    R  fetch effective address low
        4   pointer+1   R  fetch effective address high,
                           add Y to low byte of effective address
        5   address+Y*  R  read from effective address,
                           fix high byte of effective address
        6+  address+Y   R  read from effective address

       Notes: The effective address is always fetched from zero page,
              i.e. the zero page boundary crossing is not handled.

              * The high byte of the effective address may be invalid
                at this time, i.e. it may be smaller by $100.

              + This cycle will be executed only if the effective address
                was invalid during cycle #5, i.e. page boundary was crossed.

*/

void mnes_::memory::r_indirect_indexed()
{
  // 2
  uint32_t pointer = ext::read(cpureg.pc++);
  MLOG("(${:02X}), Y[{:02X}]", pointer, cpureg.y);
  // 3
  uint32_t pcl = ext::read(pointer);
  // 4
  uint32_t pch = ext::read(TO_ZERO_PAGE(pointer + 1)) << 8;
  pcl += cpureg.y;
  // 5
  // Could be invalid read
  cpureg.byteLatch = ext::read(TO_ZERO_PAGE(pcl) | pch);
  if (pcl > 0xFF) {
    // 6*, we are here because the effective address was off by $100
    cpureg.byteLatch = ext::read((pcl + pch) & 0xFFFF);
    MLOG("A:${:04X} < ${:02X}", (pcl + pch) & 0xFFFF, cpureg.byteLatch);
  } else {
    MLOG("A:${:04X} < ${:02X}", TO_ZERO_PAGE(pcl) | pch, cpureg.byteLatch);
  }
  // Do Operation
  switch (cpureg.opCode) {
    using namespace mnes::opcodes;
  case OPCODE_LDA_IN_Y:
    lda();
    break;
  case OPCODE_EOR_IN_Y:
    eor();
    break;
  case OPCODE_AND_IN_Y:
    and_();
    break;
  case OPCODE_ORA_IN_Y:
    ora();
    break;
  case OPCODE_ADC_IN_Y:
    adc();
    break;
  case OPCODE_SBC_IN_Y:
    sbc();
    break;
  case OPCODE_CMP_IN_Y:
    cmp();
    break;
  case OPCODE_LAX_IN_Y:
    lax();
    break;
  }
}
}