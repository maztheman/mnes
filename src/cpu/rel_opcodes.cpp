namespace {
bool mnes_::cpu::bcc() { return !is_carry(); }

bool mnes_::cpu::bcs() { return is_carry(); }

bool mnes_::cpu::beq() { return IF_ZERO(); }

bool mnes_::cpu::bmi() { return IF_SIGN(); }

bool mnes_::cpu::bne() { return !IF_ZERO(); }

bool mnes_::cpu::bpl() { return !IF_SIGN(); }

bool mnes_::cpu::bvc() { return !IF_OVERFLOW(); }

bool mnes_::cpu::bvs() { return IF_OVERFLOW(); }
}