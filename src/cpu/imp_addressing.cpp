namespace {
void mnes_::memory::implied_or_accumulator()
{
  ext::read(cpureg.pc);
  switch (cpureg.opCode) {
    using namespace mnes::opcodes;
  case OPCODE_CLC:
    clc();
    break;
  case OPCODE_CLD:
    cld();
    break;
  case OPCODE_CLI:
    cli();
    break;
  case OPCODE_CLV:
    clv();
    break;
  case OPCODE_DEX:
    dex();
    MLOG(" | X:${:02X}", cpureg.x);
    break;
  case OPCODE_DEY:
    dey();
    MLOG(" | Y:${:02X}", cpureg.y);
    break;
  case OPCODE_INX:
    inx();
    MLOG(" | X:${:02X}", cpureg.x);
    break;
  case OPCODE_INY:
    iny();
    MLOG(" | Y:${:02X}", cpureg.y);
    break;
  case OPCODE_NOP:
    nop();
    break;
  case OPCODE_SEC:
    sec();
    break;
  case OPCODE_SED:
    sed();
    break;
  case OPCODE_SEI:
    sei();
    break;
  case OPCODE_TAX:
    tax();
    MLOG(" | X:${:02X}", cpureg.x);
    break;
  case OPCODE_TAY:
    tay();
    MLOG(" | Y:${:02X}", cpureg.y);
    break;
  case OPCODE_TSX:
    tsx();
    MLOG(" | X:${:02X}", cpureg.x);
    break;
  case OPCODE_TXA:
    txa();
    MLOG(" | A:${:02X}", cpureg.a);
    break;
  case OPCODE_TXS:
    txs();
    MLOG(" | S:${:02X}", cpureg.stack);
    break;
  case OPCODE_TYA:
    tya();
    MLOG(" | A:${:02X}", cpureg.a);
    break;
  case OPCODE_ASL_A:
    MLOG(" | A:${:02X} ->", cpureg.a);
    internals::asl(cpureg.a);
    MLOG(" A:${:02X}", cpureg.a);
    break;
  case OPCODE_LSR_A:
    MLOG(" | A:${:02X} ->", cpureg.a);
    internals::lsr(cpureg.a);
    MLOG(" A:${:02X}", cpureg.a);
    break;
  case OPCODE_ROL_A:
    MLOG(" | A:${:02X} ->", cpureg.a);
    internals::rol(cpureg.a);
    MLOG(" A:${:02X}", cpureg.a);
    break;
  case OPCODE_ROR_A:
    MLOG(" | A:${:02X} ->", cpureg.a);
    internals::ror(cpureg.a);
    MLOG(" A:${:02X}", cpureg.a);
    break;
  }
}
}