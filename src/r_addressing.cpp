void memory_r_immediate()
{
    //2.1
    g_Registers.byteLatch = ext_memory_read(g_Registers.pc++);
    //2.2 Do Operation
    switch (g_Registers.opCode)
    {
    case OPCODE_ADC_OP:
        cpu_adc();
        break;
    case OPCODE_AND_OP:
        cpu_and();
        break;
    case OPCODE_CMP_OP:
        cpu_cmp();
        break;
    case OPCODE_CPX_OP:
        cpu_cpx();
        break;
    case OPCODE_CPY_OP:
        cpu_cpy();
        break;
    case OPCODE_EOR_OP:
        cpu_eor();
        break;
    case OPCODE_LDA_OP:
        cpu_lda();
        break;
    case OPCODE_LDX_OP:
        cpu_ldx();
        break;
    case OPCODE_LDY_OP:
        cpu_ldy();
        break;
    case OPCODE_ORA_OP:
        cpu_ora();
        break;
    case OPCODE_SBC_OP:
        cpu_sbc();
        break;
    case OPCODE_ARR_IMM:
        cpu_arr();
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
void memory_r_absolute()
{
    //2
    uint pcl = ext_memory_read(g_Registers.pc++);
    //3
    uint pch = ext_memory_read(g_Registers.pc++);
    //4.1
    g_Registers.byteLatch = ext_memory_read(pcl | (pch << 8));
    //4.2 - Do Operation
    switch (g_Registers.opCode) {

    case OPCODE_LDA_AB:
        cpu_lda();
        break;
    case OPCODE_LDX_AB:
        cpu_ldx();
        break;
    case OPCODE_LDY_AB:
        cpu_ldy();
        break; 
    case OPCODE_EOR_AB:
        cpu_eor();
        break;
    case OPCODE_AND_AB:
        cpu_and();
        break;
    case OPCODE_ORA_AB:
        cpu_ora();
        break;
    case OPCODE_ADC_AB:
        cpu_adc();
        break;
    case OPCODE_SBC_AB:
        cpu_sbc();
        break;
    case OPCODE_CMP_AB:
        cpu_cmp();
        break;
    case OPCODE_BIT_AB:
        cpu_bit();
        break;
    case OPCODE_LAX_AB:
        cpu_lax();
        break;
    case OPCODE_CPX_AB:
        cpu_cpx();
        break;
    case OPCODE_CPY_AB:
        cpu_cpy();
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

void memory_r_zero_page()
{
    //2
    uint address = ext_memory_read(g_Registers.pc++);
    //3.1
    g_Registers.byteLatch = ext_memory_read(address);
    //3.2
    switch (g_Registers.opCode) {
    case OPCODE_LDA_ZP:
        cpu_lda();
        break;
    case OPCODE_LDX_ZP:
        cpu_ldx();
        break;
    case OPCODE_LDY_ZP:
        cpu_ldy();
        break;
    case OPCODE_EOR_ZP:
        cpu_eor();
        break;
    case OPCODE_AND_ZP:
        cpu_and();
        break;
    case OPCODE_ORA_ZP:
        cpu_ora();
        break;
    case OPCODE_ADC_ZP:
        cpu_adc();
        break;
    case OPCODE_SBC_ZP:
        cpu_sbc();
        break;
    case OPCODE_CMP_ZP:
        cpu_cmp();
        break;
    case OPCODE_BIT_ZP:
        cpu_bit();
        break;
    case OPCODE_LAX_ZP:
        cpu_lax();
        break;
    case OPCODE_CPX_ZP:
        cpu_cpx();
        break;
    case OPCODE_CPY_ZP:
        cpu_cpy();
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

static inline void memory_r_zero_page_indexed(const uint& indexRegister)
{
    //2
    uint address = ext_memory_read(g_Registers.pc++);
    //3
    cpu_do_cycle();
    //4.1
    g_Registers.byteLatch = ext_memory_read((address + indexRegister) & 0xFF);
    //4.2
    switch (g_Registers.opCode) {

    case OPCODE_LDA_ZP_X:
        cpu_lda();
        break;
    case OPCODE_LDY_ZP_X:
        cpu_ldy();
        break;
    case OPCODE_EOR_ZP_X:
        cpu_eor();
        break;
    case OPCODE_AND_ZP_X:
        cpu_and();
        break;
    case OPCODE_ORA_ZP_X:
        cpu_ora();
        break;
    case OPCODE_ADC_ZP_X:
        cpu_adc();
        break;
    case OPCODE_SBC_ZP_X:
        cpu_sbc();
        break;
    case OPCODE_CMP_ZP_X:
        cpu_cmp();
        break;
    case OPCODE_LDX_ZP_Y:
        cpu_ldx();
        break;
    case OPCODE_LAX_ZP_Y:
        cpu_lax();
        break;
    }
}

void memory_r_zero_page_indexed_x()
{
    memory_r_zero_page_indexed(g_Registers.x);
}

void memory_r_zero_page_indexed_y()
{
    memory_r_zero_page_indexed(g_Registers.y);
}

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

static inline void memory_r_absolute_indexed(const uint& indexRegister)
{
    //2
    uint pcl = ext_memory_read(g_Registers.pc++);
    //3
    uint pch = ext_memory_read(g_Registers.pc++);
    pcl += indexRegister;
    //4
    g_Registers.byteLatch = ext_memory_read((pcl & 0xFF) | (pch << 8));
    if (pcl > 0xFF) {
        //5
        g_Registers.byteLatch = ext_memory_read(((pch << 8) + pcl) & 0xFFFF);
    }
}

void memory_r_absolute_indexed_x()
{
    memory_r_absolute_indexed(g_Registers.x);
    switch (g_Registers.opCode) {
    case OPCODE_LDA_AB_X:
        cpu_lda();
        break;
    case OPCODE_LDY_AB_X:
        cpu_ldy();
        break;
    case OPCODE_EOR_AB_X:
        cpu_eor();
        break;
    case OPCODE_AND_AB_X:
        cpu_and();
        break;
    case OPCODE_ORA_AB_X:
        cpu_ora();
        break;
    case OPCODE_ADC_AB_X:
        cpu_adc();
        break;
    case OPCODE_SBC_AB_X:
        cpu_sbc();
        break;
    case OPCODE_CMP_AB_X:
        cpu_cmp();
        break;
    }
}

void memory_r_absolute_indexed_y()
{
    memory_r_absolute_indexed(g_Registers.y);
    switch (g_Registers.opCode) {
    case OPCODE_LDA_AB_Y:
        cpu_lda();
        break;
    case OPCODE_LDX_AB_Y:
        cpu_ldx();
        break;
    case OPCODE_EOR_AB_Y:
        cpu_eor();
        break;
    case OPCODE_AND_AB_Y:
        cpu_and();
        break;
    case OPCODE_ORA_AB_Y:
        cpu_ora();
        break;
    case OPCODE_ADC_AB_Y:
        cpu_adc();
        break;
    case OPCODE_SBC_AB_Y:
        cpu_sbc();
        break;
    case OPCODE_CMP_AB_Y:
        cpu_cmp();
        break;
    case OPCODE_LAX_AB_Y:
        cpu_lax();
        break;
    }
}

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

void memory_r_indexed_indirect()
{
    //2
    uint pointer = ext_memory_read(g_Registers.pc++);
    //3
    cpu_do_cycle(); //pointer + x ? could be a dummy reader to pointer?
    //4
    uint pcl = ext_memory_read((pointer + g_Registers.x) & 0xFF);
    //5
    uint pch = ext_memory_read((pointer + g_Registers.x + 1) & 0xFF);
    //6.1
    g_Registers.byteLatch = ext_memory_read(pcl | (pch << 8));
    //6.2 Do Operation
    switch (g_Registers.opCode) {
    case OPCODE_LDA_IN_X:
        cpu_lda();
        break;
    case OPCODE_ORA_IN_X:
        cpu_ora();
        break;
    case OPCODE_EOR_IN_X:
        cpu_eor();
        break;
    case OPCODE_AND_IN_X:
        cpu_and();
        break;
    case OPCODE_ADC_IN_X:
        cpu_adc();
        break;
    case OPCODE_CMP_IN_X:
        cpu_cmp();
        break;
    case OPCODE_SBC_IN_X:
        cpu_sbc();
        break;
    case OPCODE_LAX_IN_X:
        cpu_lax();
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

void memory_r_indirect_indexed()
{
    //2
    uint pointer = ext_memory_read(g_Registers.pc++);
    //3
    uint pcl = ext_memory_read(pointer);
    //4
    uint pch = ext_memory_read((pointer + 1) & 0xFF) << 8;
    pcl += g_Registers.y;
    //5
    g_Registers.byteLatch = ext_memory_read((pcl & 0xFF) | pch);
    if (pcl > 0xFF) {
        //6*
        g_Registers.byteLatch = ext_memory_read((pcl + pch) & 0xFFFF);
    }
    //Do Operation
    switch (g_Registers.opCode)
    {
    case OPCODE_LDA_IN_Y:
        cpu_lda();
        break;
    case OPCODE_EOR_IN_Y:
        cpu_eor();
        break;
    case OPCODE_AND_IN_Y:
        cpu_and();
        break;
    case OPCODE_ORA_IN_Y:
        cpu_ora();
        break;
    case OPCODE_ADC_IN_Y:
        cpu_adc();
        break;
    case OPCODE_SBC_IN_Y:
        cpu_sbc();
        break;
    case OPCODE_CMP_IN_Y:
        cpu_cmp();
        break;
    }
}