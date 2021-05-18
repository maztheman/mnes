/*
* Read-Write-Modify Instructions
*
*
* ASL, LSR, ROL, ROR, INC, DEC,
* SLO, SRE, RLA, RRA, ISB, DCP
*
*

/*
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
static inline void memory_rmw_absolute_indexed()
{
    //2
    uint pcl = ext_memory_read(g_Registers.pc++);
    //3
    uint pch = ext_memory_read(g_Registers.pc);
    pcl += g_Registers.x;
    g_Registers.pc++;
    //4
    ext_memory_read((pch << 8) | (pcl & 0xFF));
    pch += (pcl >> 8); //pcl could be 0x100
    pch &= 0xFF;
    g_Registers.addressLatch = ((pch << 8) | (pcl & 0xFF));
    //5
    g_Registers.byteLatch = ext_memory_read(g_Registers.addressLatch);
    //6.1
    ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
    //6.2 do operation
    switch (g_Registers.opCode) 
    {
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
        cpu_slo();
        break;
    case OPCODE_LSE_AB_X:
        cpu_lse();
        break;
    case OPCODE_RLA_AB_X:
        cpu_rla();
        break;
    case OPCODE_RRA_AB_X:
        cpu_rra();
        break;
    case OPCODE_ISC_AB_X:
        cpu_isc();
        break;
    case OPCODE_DCP_AB_X:
        cpu_dcp();
        break;
    }
    //7
    ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
}

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
    //2
    uint pcl = ext_memory_read(g_Registers.pc++);
    //3
    uint pch = ext_memory_read(g_Registers.pc++);
    g_Registers.addressLatch = (pch << 8) | pcl;
    //4
    g_Registers.byteLatch = ext_memory_read(g_Registers.addressLatch);
    //5.1
    ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
    //5.1 Do the operation
    switch (g_Registers.opCode)     {
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
    //6
    ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
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
    //2
    g_Registers.addressLatch = ext_memory_read(g_Registers.pc++);
    //3
    g_Registers.byteLatch = ext_memory_read(g_Registers.addressLatch);
    //4.1
    ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
    //4.2 Do the operation
    switch (g_Registers.opCode) {
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
    //5
    ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
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
    //2
    uint address = ext_memory_read(g_Registers.pc++);
    //3
    uint effectiveAddress = (ext_memory_read(address) + g_Registers.x) & 0xFF;
    //4
    g_Registers.byteLatch = ext_memory_read(effectiveAddress);
    //5.1
    ext_memory_write(effectiveAddress, g_Registers.byteLatch);
    //5.2 Do Operation
    switch (g_Registers.opCode) {
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
    //6
    ext_memory_write(effectiveAddress, g_Registers.byteLatch);
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
    //2
    uint pointer = ext_memory_read(g_Registers.pc++);
    //3
    cpu_do_cycle();//read from the address, add X to it, this simulates that
    //4
    uint pcl = ext_memory_read((pointer + g_Registers.x) & 0xFF);
    //5
    uint pch = ext_memory_read((pointer + g_Registers.x + 1) & 0xFF);
    g_Registers.addressLatch = pcl | (pch << 8);
    //6
    g_Registers.byteLatch = ext_memory_read(g_Registers.addressLatch);
    //7.1
    ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
    //7.2
    switch (g_Registers.opCode)
    {
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

    }
    //8
    ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
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
    //2
    uint pointer = ext_memory_read(g_Registers.pc++);
    //3
    uint pcl = ext_memory_read(pointer);
    //4
    uint pch = ext_memory_read((pointer + 1) & 0xFF);
    pcl += g_Registers.y;
    //5
    ext_memory_read((pcl & 0xFF) | (pch << 8));
    g_Registers.addressLatch = ((pch << 8) + pcl) & 0xFFFF;
    //6
    g_Registers.byteLatch = ext_memory_read(g_Registers.addressLatch);
    //7.1
    ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
    //7.2 - Do Operation
    switch (g_Registers.opCode)     {
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
    }
    //8
    ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
}