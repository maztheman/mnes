static inline void memory_immediate()
{
	//2.1
	GRegisters().byteLatch = ext_memory_read(GRegisters().pc++);
}