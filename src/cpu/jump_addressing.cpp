/*
     JMP

        #  address R/W description
       --- ------- --- -------------------------------------------------
        1    PC     R  fetch opcode, increment PC
        2    PC     R  fetch low address byte, increment PC
        3    PC     R  copy low address byte to PCL, fetch high address
                       byte to PCH
*/
namespace {

void mnes_::memory::jmp_absolute()
{
  // 2
  uint pcl = ext::read(cpureg.pc++);
  // 3
  cpureg.pc = pcl | (ext::read(cpureg.pc) << 8);
#ifdef USE_LOG
  VLog().AddLine(" $%04X", cpureg.pc);
#endif
}

/*
  Absolute indirect addressing (JMP)

        #   address  R/W description
       --- --------- --- ------------------------------------------
        1     PC      R  fetch opcode, increment PC
        2     PC      R  fetch pointer address low, increment PC
        3     PC      R  fetch pointer address high, increment PC
        4   pointer   R  fetch low address to latch
        5  pointer+1* R  fetch PCH, copy latch to PCL

       Note: * The PCH will always be fetched from the same page
               than PCL, i.e. page boundary crossing is not handled.
*/

void mnes_::memory::jmp_absolute_indirect()
{
  // 2
  uint pointer_l = ext::read(cpureg.pc++);
  // 3
  uint pointer_h = ext::read(cpureg.pc++) << 8;
  // 4
  uint pcl = ext::read(pointer_l | pointer_h);
  // 5
  cpureg.pc = pcl | (ext::read(((pointer_l + 1) & 0xFF) | pointer_h) << 8);
}
}