static inline void memory_immediate()
{
	//2.1
	g_Registers.byteLatch = ext_memory_read(g_Registers.pc++);
}