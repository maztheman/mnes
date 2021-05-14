static inline void cpu_bcc() {
	g_Registers.pc += 2;
	uint nOld = g_Registers.pc;

	if (!is_carry()) {
		g_Registers.pc += (char)g_Registers.byteLatch;//signed byte
		cpu_do_cycle();
		g_Registers.extraCycles++;
	}

	g_Registers.pc &= 0xFFFF;

	if ((nOld & 0xFF00) != (g_Registers.pc & 0xFF00)) {
		cpu_do_cycle();
		g_Registers.extraCycles++;
	}
}

static inline void cpu_bcs() {
	g_Registers.pc += 2;
	uint nOld = g_Registers.pc;

	if (is_carry()) {
		g_Registers.pc += (char)g_Registers.byteLatch;//signed byte
		cpu_do_cycle();
		g_Registers.extraCycles++;
	}

	g_Registers.pc &= 0xFFFF;

	if ((nOld & 0xFF00) != (g_Registers.pc & 0xFF00)) {
		cpu_do_cycle();
		g_Registers.extraCycles++;
	}
}

static inline void cpu_beq() {
	g_Registers.pc += 2;
	uint nOld = g_Registers.pc;

	if (IF_ZERO()) {
		g_Registers.pc += (char)g_Registers.byteLatch;//signed byte
		cpu_do_cycle();
		g_Registers.extraCycles++;
	}

	g_Registers.pc &= 0xFFFF;

	if ((nOld & 0xFF00) != (g_Registers.pc & 0xFF00)) {
		cpu_do_cycle();
		g_Registers.extraCycles++;
	}
}

static inline void cpu_bmi() {
	g_Registers.pc += 2;
	uint nOld = g_Registers.pc;

	if (IF_SIGN()) {
		g_Registers.pc += (char)g_Registers.byteLatch;//signed byte
		cpu_do_cycle();
		g_Registers.extraCycles++;
	}

	g_Registers.pc &= 0xFFFF;

	if ((nOld & 0xFF00) != (g_Registers.pc & 0xFF00)) {
		cpu_do_cycle();
		g_Registers.extraCycles++;
	}
}

static inline void cpu_bne() {
	g_Registers.pc += 2;
	uint nOld = g_Registers.pc;

	if (!IF_ZERO()) {
		g_Registers.pc += (char)g_Registers.byteLatch;//signed byte
		cpu_do_cycle();
		g_Registers.extraCycles++;
	}

	g_Registers.pc &= 0xFFFF;

	if ((nOld & 0xFF00) != (g_Registers.pc & 0xFF00)) {
		cpu_do_cycle();
		g_Registers.extraCycles++;
	}
}

static inline void cpu_bpl() {
	g_Registers.pc += 2;
	uint nOld = g_Registers.pc;

	if (!IF_SIGN()) {
		g_Registers.pc += (char)g_Registers.byteLatch;//signed byte
		cpu_do_cycle();
		g_Registers.extraCycles++;
	}

	g_Registers.pc &= 0xFFFF;

	if ((nOld & 0xFF00) != (g_Registers.pc & 0xFF00)) {
		cpu_do_cycle();
		g_Registers.extraCycles++;
	}
}

static inline void cpu_bvc() {
	g_Registers.pc += 2;
	uint nOld = g_Registers.pc;

	if (!IF_OVERFLOW()) {
		g_Registers.pc += (char)g_Registers.byteLatch;//signed byte
		cpu_do_cycle();
		g_Registers.extraCycles++;
	}

	g_Registers.pc &= 0xFFFF;

	if ((nOld & 0xFF00) != (g_Registers.pc & 0xFF00)) {
		cpu_do_cycle();
		g_Registers.extraCycles++;
	}
}

static inline void cpu_bvs() {
	g_Registers.pc += 2;
	uint nOld = g_Registers.pc;

	if (IF_OVERFLOW()) {
		g_Registers.pc += (char)g_Registers.byteLatch;//signed byte
		cpu_do_cycle();
		g_Registers.extraCycles++;
	}

	g_Registers.pc &= 0xFFFF;

	if ((nOld & 0xFF00) != (g_Registers.pc & 0xFF00)) {
		cpu_do_cycle();
		g_Registers.extraCycles++;
	}
}