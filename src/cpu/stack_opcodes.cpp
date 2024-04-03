/*
         BRK

                #  address R/W description
           --- ------- --- -----------------------------------------------
                1    PC     R  fetch opcode, increment PC
                2    PC     R  read next instruction byte (and throw it away),
                                           increment PC
                3  $0100,S  W  push PCH on stack (with B flag set), decrement S
                4  $0100,S  W  push PCL on stack, decrement S
                5  $0100,S  W  push P on stack, decrement S
                6   $FFFE   R  fetch PCL
                7   $FFFF   R  fetch PCH
*/

namespace {

void mnes_::cpu::brk()
{
  uint pcl_addr = 0;
  uint pch_addr = 0;
  // 2
  ext::read(cpureg.pc);
  if (cpureg.actual_irq == doing_irq::brk) { cpureg.pc++; }
  // 3-4
  push_pc();
  // Poll the most up to date irq lines so we can interrupt
  if (cpureg.nmi || cpureg.actual_irq == doing_irq::nmi) {
    cpureg.nmi = false;
    pcl_addr = NMILO;
    pch_addr = NMIHI;
#ifdef USE_LOG
    if (cpureg.actual_irq == doing_irq::brk) {
      VLog().AddLine("** NMI INTERRUPTED BRK **\n");
    } else if (cpureg.actual_irq == doing_irq::irq) {
      VLog().AddLine("** NMI INTERRUPTED IRQ **\n");
    }
#endif
  } else {
    pcl_addr = BRKLO;
    pch_addr = BRKHI;

#ifdef USE_LOG
    if (cpureg.actual_irq == doing_irq::brk) { VLog().AddLine("** IRQ INTERRUPTED BRK **\n"); }
#endif
  }
  // 5
  push_byte((cpureg.actual_irq != doing_irq::brk) ? (cpureg.status | BFLAG_10_MASK)
                                                               : (cpureg.status | BFLAG_10_MASK | BFLAG_01_MASK));
  // 6
  cpureg.pc = (cpureg.pc & 0xFF00) | ext::read(pcl_addr);
  SET_INTERRUPT(true);
  // 7
  cpureg.pc = (cpureg.pc & 0x00FF) | (ext::read(pch_addr) << 8);
}

/*
        RTI

                #  address R/W description
           --- ------- --- -----------------------------------------------
                1    PC     R  fetch opcode, increment PC
                2    PC     R  read next instruction byte (and throw it away)
                3  $0100,S  R  increment S
                4  $0100,S  R  pull P from stack, increment S
                5  $0100,S  R  pull PCL from stack, increment S
                6  $0100,S  R  pull PCH from stack
*/

void mnes_::cpu::rti()
{
  // 2
  ext::read(cpureg.pc);
  // 3
  do_cycle();
  inc_stack();
  // 4
  cpureg.status = ext::read(0x100 + cpureg.stack) & BFLAG_CLEAR_MASK;
  inc_stack();
  // 5
  cpureg.pc = (cpureg.pc & 0xFF00) | ext::read(0x100 + cpureg.stack);
  inc_stack();
  // 6
  cpureg.pc = (cpureg.pc & 0x00FF) | (ext::read(0x100 + cpureg.stack) << 8);
}

/*
         RTS

                #  address R/W description
           --- ------- --- -----------------------------------------------
                1    PC     R  fetch opcode, increment PC
                2    PC     R  read next instruction byte (and throw it away)
                3  $0100,S  R  increment S
                4  $0100,S  R  pull PCL from stack, increment S
                5  $0100,S  R  pull PCH from stack
                6    PC     R  increment PC
*/

void mnes_::cpu::rts()
{
  // 2
  ext::read(cpureg.pc);
  // 3
  do_cycle();
  inc_stack();
  // 4
  cpureg.pc = (cpureg.pc & 0xFF00) | ext::read(0x100 + cpureg.stack);
  inc_stack();
  // 5
  cpureg.pc = (cpureg.pc & 0x00FF) | (ext::read(0x100 + cpureg.stack) << 8);
  // 6
  do_cycle();
  cpureg.pc++;
}

/*
         PHA, PHP

                #  address R/W description
           --- ------- --- -----------------------------------------------
                1    PC     R  fetch opcode, increment PC
                2    PC     R  read next instruction byte (and throw it away)
                3  $0100,S  W  push register on stack, decrement S
*/

void mnes_::cpu::pha()
{
  // 2
  ext::read(cpureg.pc);
  // 3
  push_byte(cpureg.a);
}

void mnes_::cpu::php()
{
  // 2
  ext::read(cpureg.pc);
  // 3
  push_byte(cpureg.status | BFLAG_01_MASK | BFLAG_10_MASK);
}

/*
         PLA, PLP

                #  address R/W description
           --- ------- --- -----------------------------------------------
                1    PC     R  fetch opcode, increment PC
                2    PC     R  read next instruction byte (and throw it away)
                3  $0100,S  R  increment S
                4  $0100,S  R  pull register from stack
*/

void mnes_::cpu::pla()
{
  // 2
  ext::read(cpureg.pc);
  // 3
  do_cycle();
  inc_stack();
  // 4
  cpureg.a = ext::read(0x100 + cpureg.stack);
  set_nz(cpureg.a);
}

void mnes_::cpu::plp()
{
  // 2
  ext::read(cpureg.pc);
  // 3
  do_cycle();
  inc_stack();
  // 4
  bool old = IF_INTERRUPT();
  cpureg.status = ext::read(0x100 + cpureg.stack) & BFLAG_CLEAR_MASK;
  cpureg.delayed = IF_INTERRUPT() ? delayed_i::yes : delayed_i::no;
  SET_INTERRUPT(old);
}

/*
         JSR

                #  address R/W description
           --- ------- --- -------------------------------------------------
                1    PC     R  fetch opcode, increment PC
                2    PC     R  fetch low address byte, increment PC
                3  $0100,S  R  internal operation (predecrement S?)
                4  $0100,S  W  push PCH on stack, decrement S
                5  $0100,S  W  push PCL on stack, decrement S
                6    PC     R  copy low address byte to PCL, fetch high address
                                           byte to PCH
*/

void mnes_::cpu::jsr()
{
  // 2
  uint pcl = ext::read(cpureg.pc++);
  // 3
  do_cycle();
  // 4-5
  push_pc();
  // 6
  cpureg.pc = pcl | (ext::read(cpureg.pc) << 8);
  MLOG(" $%04X", cpureg.pc);
}

}