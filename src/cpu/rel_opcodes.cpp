static inline bool cpu_bcc() 
{
	return !is_carry();
}

static inline bool cpu_bcs() 
{
	return is_carry();
}

static inline bool cpu_beq() 
{
	return IF_ZERO();
}

static inline bool cpu_bmi() 
{
	return IF_SIGN();
}

static inline bool cpu_bne() 
{
	return !IF_ZERO();
}

static inline bool cpu_bpl() 
{
	return !IF_SIGN();
}

static inline bool cpu_bvc() 
{
	return !IF_OVERFLOW();
}

static inline bool cpu_bvs() 
{
	return IF_OVERFLOW();
}