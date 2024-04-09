/*
STA, STX, STY, SHA, SHX, SHY

*/
namespace {
void mnes_::cpu::sta() { cpureg.byteLatch = cpureg.a; }

void mnes_::cpu::stx() { cpureg.byteLatch = cpureg.x; }

void mnes_::cpu::sty() { cpureg.byteLatch = cpureg.y; }

void mnes_::cpu::sax() { cpureg.byteLatch = cpureg.a & cpureg.x; }

/*
void mnes_::cpu::axs()
{
        //immediate mode value used later
        uint32_t tmp = cpureg.byteLatch;
        //used to restore a
        uint32_t oldA = cpureg.a;
        //copies a into byteLatch
        cpu_sta();
        //copies x into a
        cpu_txa();
        //ands a with byteLatch (bascially a & x)
        cpu_and();
        cpu_sec();
        cpureg.byteLatch = tmp;
        cpu_sbc();
        cpu_tax();
        //restore a
        cpureg.byteLatch = oldA;
        cpu_lda();
}
*/
}