namespace {
void mnes_::memory::immediate()
{
  // 2.1
  cpureg.byteLatch = ext::read(cpureg.pc++);
}
}