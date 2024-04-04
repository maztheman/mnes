/*
STA, STX, STY, SHA, SHX, SHY

*/

static inline void cpu_sta() { GRegisters().byteLatch = GRegisters().a; }

static inline void cpu_stx() { GRegisters().byteLatch = GRegisters().x; }

static inline void cpu_sty() { GRegisters().byteLatch = GRegisters().y; }

static inline void cpu_sax() { GRegisters().byteLatch = GRegisters().a & GRegisters().x; }

/*
static inline void cpu_axs()
{
        //immediate mode value used later
        uint tmp = GRegisters().byteLatch;
        //used to restore a
        uint oldA = GRegisters().a;
        //copies a into byteLatch
        cpu_sta();
        //copies x into a
        cpu_txa();
        //ands a with byteLatch (bascially a & x)
        cpu_and();
        cpu_sec();
        GRegisters().byteLatch = tmp;
        cpu_sbc();
        cpu_tax();
        //restore a
        GRegisters().byteLatch = oldA;
        cpu_lda();
}
*/