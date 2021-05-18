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

static inline void cpu_brk() 
{
	uint pcl_addr = 0;
	uint pch_addr = 0;
	bool is_irq = false;
	bool is_nmi = false;
	if (g_Registers.prev_nmi) {
		g_Registers.nmi = false;
		is_nmi = true;
		pcl_addr = NMILO;
		pch_addr = NMIHI;
	} else if (g_Registers.prev_irq) {
		is_irq = true;
		pcl_addr = BRKLO;
		pch_addr = BRKHI;
	} else {
		pcl_addr = BRKLO;
		pch_addr = BRKHI;
	}
	//2
	ext_memory_read(g_Registers.pc);
	if (is_irq == false && is_nmi == false) {
		g_Registers.pc++;
	}
	//3-4
	memory_push_pc();
	//5
	memory_push_byte((is_nmi || is_irq) ? (g_Registers.status & 0xEF) : (g_Registers.status | 0x10));
	//6
	g_Registers.pc = (g_Registers.pc & 0xFF00) | ext_memory_read(pcl_addr);
	SET_INTERRUPT(true);
	//7
	g_Registers.pc = (g_Registers.pc & 0x00FF) | (ext_memory_read(pch_addr) << 8);
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

static inline void cpu_rti()
{
	//2
	ext_memory_read(g_Registers.pc + 1);
	//3
	cpu_do_cycle();
	memory_inc_stack();
	//4
	g_Registers.status = ext_memory_read(0x100 + g_Registers.stack);
	memory_inc_stack();
	//5
	g_Registers.pc = (g_Registers.pc & 0xFF00) |  ext_memory_read(0x100 + g_Registers.stack);
	memory_inc_stack();
	//6
	g_Registers.pc = (g_Registers.pc & 0x00FF) | (ext_memory_read(0x100 + g_Registers.stack) << 8);
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

static inline void cpu_rts() 
{
	//2
	ext_memory_read(g_Registers.pc + 1);
	//3
	cpu_do_cycle();
	memory_inc_stack();
	//4
	g_Registers.pc = (g_Registers.pc & 0xFF00) | ext_memory_read(0x100 + g_Registers.stack);
	memory_inc_stack();
	//5
	g_Registers.pc = (g_Registers.pc & 0x00FF) | (ext_memory_read(0x100 + g_Registers.stack) << 8);
	//6
	cpu_do_cycle();
	g_Registers.pc++;
}

/*
	 PHA, PHP

		#  address R/W description
	   --- ------- --- -----------------------------------------------
		1    PC     R  fetch opcode, increment PC
		2    PC     R  read next instruction byte (and throw it away)
		3  $0100,S  W  push register on stack, decrement S
*/

static inline void cpu_pha()
{
	//2
	ext_memory_read(g_Registers.pc + 1);
	//3
	memory_push_byte(g_Registers.a);
}

static inline void cpu_php()
{
	//2
	ext_memory_read(g_Registers.pc + 1);
	//3
	memory_push_byte(g_Registers.status);
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

static inline void cpu_pla()
{
	//2
	ext_memory_read(g_Registers.pc + 1);
	//3
	cpu_do_cycle();
	memory_inc_stack();
	//4
	g_Registers.a = ext_memory_read(0x100 + g_Registers.stack);
	set_nz(g_Registers.a);
}

static inline void cpu_plp()
{
	//delay of irq will not work right now, gotta figure it out
	//2
	ext_memory_read(g_Registers.pc + 1);
	//3
	cpu_do_cycle();
	memory_inc_stack();
	//4
	g_Registers.status = ext_memory_read(0x100 + g_Registers.stack);;
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

static inline void cpu_jsr() 
{
	//2
	uint pcl = ext_memory_read(g_Registers.pc++);
	//3
	cpu_do_cycle();
	//4-5
	memory_push_pc();
	//6
	g_Registers.pc = pcl | (ext_memory_read(g_Registers.pc) << 8);
}
