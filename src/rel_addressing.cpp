/*
  Relative addressing (BCC, BCS, BNE, BEQ, BPL, BMI, BVC, BVS)

        #   address  R/W description
       --- --------- --- ---------------------------------------------
        1     PC      R  fetch opcode, increment PC
        2     PC      R  fetch operand, increment PC
        3     PC      R  Fetch opcode of next instruction,
                         If branch is taken, add operand to PCL.
                         Otherwise increment PC.
        4+    PC*     R  Fetch opcode of next instruction.
                         Fix PCH. If it did not change, increment PC.
        5!    PC      R  Fetch opcode of next instruction,
                         increment PC.

       Notes: The opcode fetch of the next instruction is included to
              this diagram for illustration purposes. When determining
              real execution times, remember to subtract the last
              cycle.

              * The high byte of Program Counter (PCH) may be invalid
                at this time, i.e. it may be smaller or bigger by $100.

              + If branch is taken, this cycle will be executed.

              ! If branch occurs to different page, this cycle will be
                executed.


That's because the "last cycle" is actually "Fetch opcode of next instruction"
<_Q> A non-taken branch is 2 cycles
<_Q> A taken branch that doesn't cross a page boundary is 3 cycles
<_Q> And a taken branch that _does_ cross a page boundary is 4 cycles

*/

void memory_rel()
{
    //2.1
    uint operand = ext_memory_read(g_Registers.pc++);
    MLOG(" $%02X %ld", operand, (int)operand);
    //2.2 Do Operation
    bool bTakeBranch = false;
    switch (g_Registers.opCode) 
    {
    case OPCODE_BCC:
        bTakeBranch = cpu_bcc();
        break;
    case OPCODE_BCS:
        bTakeBranch = cpu_bcs();
        break;
    case OPCODE_BNE:
        bTakeBranch = cpu_bne();
        break;
    case OPCODE_BEQ:
        bTakeBranch = cpu_beq();
        break;
    case OPCODE_BPL:
        bTakeBranch = cpu_bpl();
        break;
    case OPCODE_BMI:
        bTakeBranch = cpu_bmi();
        break;
    case OPCODE_BVC:
        bTakeBranch = cpu_bvc();
        break;
    case OPCODE_BVS:
        bTakeBranch = cpu_bvs();
        break;
    }
    if (bTakeBranch) {
        //Before 3rd cycle, we split the pc into pcl and pch
        int pcl = (int)(g_Registers.pc & 0xFF);
        int pch = (int)(g_Registers.pc & 0xFF00);
        //3.1
        ext_memory_read(g_Registers.pc);
        //3.2 add operand to PCL
        pcl += (int)operand;
        //4.1 - read, could be invalid, this is how CPU fixes PCL zero page boundaries.
        ext_memory_read(TO_ZERO_PAGE(pcl) | pch);
        //4.2 Fix PCH
        pch = (pcl + pch) & 0xFFFF;
        if ((pch & 0xFF00) == (g_Registers.pc & 0xFF00)) {
            //4.3 if they didnt change increase PC
            pch = (pch + 1) & 0xFFFF;
        }
        g_Registers.pc = pch;
    }
}
