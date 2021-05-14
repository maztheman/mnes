static inline void cpu_clc() {
	set_carry(false);
}

static inline void cpu_cld() {
	SET_DECIMAL(false);
}

static inline void cpu_cli() {
	SET_INTERRUPT(false);
}

static inline void cpu_clv() {
	SET_OVERFLOW(false);
}

static inline void cpu_sec() {
	set_carry(true);
}

static inline void cpu_sed() {
	SET_DECIMAL(true);//maybe we should do nothing...cuz this is illegal on nes
}

static inline void cpu_sei() {
	SET_INTERRUPT(true);
}
