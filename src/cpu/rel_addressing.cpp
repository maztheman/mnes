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
  // 2.1
  int8_t operand = static_cast<int8_t>(ext_memory_read(GRegisters().pc++) & 0xFF);
  MLOG(" $%02X %d", operand & 0xFF, operand);
  // 2.2 Do Operation
  bool bTakeBranch = false;
  switch (GRegisters().opCode) {
    using namespace mnes::opcodes;
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
    MLOG(" [Taken]");
    // Here is a good example of bad stuff happening due to pcl wrapping around
    //$F228 10 BPL $D0 -48 [Taken] pcl: FFFFFFFA pch: FFFA T:[$000000000001DD5C]
    // I think this is more correct:
    //$F228 10 BPL $D0 -48 [Taken] pcl: -6 [FFFFFFFA] pch: F1FA T:[$0000000000049CEA]


    // Before 3rd cycle, we split the pc into pcl and pch
    int pcl = static_cast<int>(GRegisters().pc & 0xFF);
    int pch = static_cast<int>(GRegisters().pc & 0xFF00);
    // 3.1
    ext_memory_read(GRegisters().pc);
    // 3.2 add operand to PCL
    pcl += static_cast<int>(operand);
    MLOG(" pcl: %d [%04X]", pcl, pcl);

    // If the next opcode is in a different page, then read that open code (use a cycle)
    if (pcl < 0 || pcl > 0xFF) {
      // 4.1 - read, could be invalid, this is how CPU fixes PCL zero page boundaries.
      ext_memory_read(TO_ZERO_PAGE(pcl) | static_cast<uint>(pch));
      // 4.2 Fix PCH
    }
    pch = (static_cast<int>(GRegisters().pc) + operand) & 0xFFFF;
    // pch = (pcl | pch) & 0xFFFF;
    MLOG(" pch: %04X", pch);
    // if ((pch & 0xFF00) == (GRegisters().pc & 0xFF00)) {
    // this next step should be done, however in this emulator the opcode fetch is done in the "next" iteration
    // in order to fix this I would need to be able to know if a opcode has been fetched already
    // 4.3 if they didnt change increase PC
    // pch = (pch + 1) & 0xFFFF;
    //}
    GRegisters().pc = static_cast<uint>(pch);
  }
#ifdef USE_LOG
  else {
    MLOG(" [Not Taken]");
  }
#endif
}
