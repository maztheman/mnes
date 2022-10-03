/*
Write instructions(STA, STX, STY, SAX)

#  address R / W description
-- - ------ - -- - ------------------------------------------
1    PC     R  fetch opcode, increment PC
2    PC     R  fetch low byte of address, increment PC
3    PC     R  fetch high byte of address, increment PC
4  address  W  write register to effective address
*/

void memory_w_absolute()
{
	//2
	uint pcl = ext_memory_read(g_Registers.pc++);
	//3
	uint pch = ext_memory_read(g_Registers.pc++) << 8;
	//4.1 Do Operation
	switch (g_Registers.opCode)
	{
	case OPCODE_STA_AB:
		cpu_sta();
		break;
	case OPCODE_STX_AB:
		cpu_stx();
		break;
	case OPCODE_STY_AB:
		cpu_sty();
		break;
	}
	//4.2 
	MLOG(" $%04X <= $%02X", pcl | pch, g_Registers.byteLatch);
	ext_memory_write(pcl | pch, g_Registers.byteLatch);
}

/*
	 Write instructions (STA, STX, STY, SAX)

		#  address R/W description
	   --- ------- --- ------------------------------------------
		1    PC     R  fetch opcode, increment PC
		2    PC     R  fetch address, increment PC
		3  address  W  write register to effective address
*/

void memory_w_zero_page()
{
	//2
	uint address = ext_memory_read(g_Registers.pc++);
	//3.1
	switch (g_Registers.opCode) 	{
	case OPCODE_STA_ZP:
		cpu_sta();
		break;
	case OPCODE_STX_ZP:
		cpu_stx();
		break;
	case OPCODE_STY_ZP:
		cpu_sty();
		break;
	case OPCODE_SAX_ZP:
		cpu_sax();
		break;
	}
	//3.2
	MLOG(" $%02X <= $%02X", address, g_Registers.byteLatch)

	ext_memory_write(address, g_Registers.byteLatch);
}

/*
	 Write instructions (STA, STX, STY, SAX)

		#   address  R/W description
	   --- --------- --- -------------------------------------------
		1     PC      R  fetch opcode, increment PC
		2     PC      R  fetch address, increment PC
		3   address   R  read from address, add index register to it
		4  address+I* W  write to effective address

	   Notes: I denotes either index register (X or Y).

			  * The high byte of the effective address is always zero,
				i.e. page boundary crossings are not handled.
*/

static inline void memory_w_zero_page_indexed(const uint& indexRegister)
{
	//2
	uint address = ext_memory_read(g_Registers.pc++);
	//3.1 read from address
	ext_memory_read(address);
	MLOG(" $%02X, I[$%02X]", address, indexRegister);
	//3.2 add index register to address
	address = TO_ZERO_PAGE(address + indexRegister);
	//4.1 Do Operation
	switch (g_Registers.opCode) {
	case OPCODE_STA_ZP_X:
		cpu_sta();
		break;
	case OPCODE_STY_ZP_X:
		cpu_sty();
		break;
	case OPCODE_STX_ZP_Y:
		cpu_stx();
		break;
	case OPCODE_SAX_ZP_Y:
		cpu_sax();
		break;
	}
	//4.2
	MLOG(" A:$%02X <= $%02X", address, g_Registers.byteLatch)
	ext_memory_write(address, g_Registers.byteLatch);
}

void memory_w_zero_page_indexed_x()
{
	memory_w_zero_page_indexed(g_Registers.x);
}

void memory_w_zero_page_indexed_y()
{
	memory_w_zero_page_indexed(g_Registers.y);
}

/*
	 Write instructions (STA, STX, STY, SHA, SHX, SHY)

		#   address  R/W description
	   --- --------- --- ------------------------------------------
		1     PC      R  fetch opcode, increment PC
		2     PC      R  fetch low byte of address, increment PC
		3     PC      R  fetch high byte of address,
						 add index register to low address byte,
						 increment PC
		4  address+I* R  read from effective address,
						 fix the high byte of effective address
		5  address+I  W  write to effective address

	   Notes: I denotes either index register (X or Y).

			  * The high byte of the effective address may be invalid
				at this time, i.e. it may be smaller by $100. Because
				the processor cannot undo a write to an invalid
				address, it always reads from the address first.
*/

void memory_w_absolute_indexed(const uint& indexRegister)
{
	//2
	uint pcl = ext_memory_read(g_Registers.pc++);
	//3.1 fetch high byte of address
	uint pch = ext_memory_read(g_Registers.pc++) << 8;
	MLOG(" $%04X, I[$%02X]", pcl | pch, indexRegister);
	//3.2 add index register to low address byte
	pcl += indexRegister;
	//3.3 increase PC (already done)
	//4.1 read from effective address
	ext_memory_read(TO_ZERO_PAGE(pcl) | pch);
	//4.2 fix high byte
	uint address = (pcl + pch) & 0xFFFF;
	//5.1 Do Operation
	switch (g_Registers.opCode)
	{
	case OPCODE_STA_AB_X:
	case OPCODE_STA_AB_Y:
		cpu_sta();
		break;
	case OPCODE_SHY_AB_X:
		g_Registers.byteLatch = (g_Registers.y & ((address >> 8) + 1)) & 0xFF;
		if ((g_Registers.x + pcl) > 0xFF) {
			return;
		}
		break;
	case OPCODE_SHX_AB_Y:
		g_Registers.byteLatch = (g_Registers.x & ((address >> 8) + 1)) & 0xFF;
		if ((g_Registers.y + pcl) > 0xFF) {
			return;
		}
		break;
	}
	//5.2
	MLOG(" A:$%04X <= $%02X", address, g_Registers.byteLatch)
	ext_memory_write(address, g_Registers.byteLatch);
}

void memory_w_absolute_indexed_x()
{
	memory_w_absolute_indexed(g_Registers.x);
}

void memory_w_absolute_indexed_y()
{
	memory_w_absolute_indexed(g_Registers.y);
}

/*
	 Write instructions (STA, SAX)

		#    address   R/W description
	   --- ----------- --- ------------------------------------------
		1      PC       R  fetch opcode, increment PC
		2      PC       R  fetch pointer address, increment PC
		3    pointer    R  read from the address, add X to it
		4   pointer+X   R  fetch effective address low
		5  pointer+X+1  R  fetch effective address high
		6    address    W  write to effective address

	   Note: The effective address is always fetched from zero page,
			 i.e. the zero page boundary crossing is not handled.
*/

void memory_w_indexed_indirect()
{
	//2
	uint pointer = ext_memory_read(g_Registers.pc++);
	MLOG(" ($%02X, X[$%02X])", pointer, g_Registers.x)
	//3.1 read from pointer address, throw away data
	ext_memory_read(pointer);
	//3.2 add X to pointer address
	pointer += g_Registers.x;
	//4
	uint pcl = ext_memory_read(TO_ZERO_PAGE(pointer));
	//5
	uint pch = ext_memory_read(TO_ZERO_PAGE(pointer + 1)) << 8;
	//6.1 Do Operation
	switch (g_Registers.opCode)
	{
	case OPCODE_STA_IN_X:
		cpu_sta();
		break;
	}
	//6.2
	MLOG(" A:$%04X <= $%02X", pcl | pch, g_Registers.byteLatch)
	ext_memory_write(pcl | pch, g_Registers.byteLatch);
}
/*
	 Write instructions (STA, SHA)

		#    address   R/W description
	   --- ----------- --- ------------------------------------------
		1      PC       R  fetch opcode, increment PC
		2      PC       R  fetch pointer address, increment PC
		3    pointer    R  fetch effective address low
		4   pointer+1   R  fetch effective address high,
						   add Y to low byte of effective address
		5   address+Y*  R  read from effective address,
						   fix high byte of effective address
		6   address+Y   W  write to effective address

	   Notes: The effective address is always fetched from zero page,
			  i.e. the zero page boundary crossing is not handled.

			  * The high byte of the effective address may be invalid
				at this time, i.e. it may be smaller by $100.
*/

void memory_w_indirect_indexed()
{
	//2
	uint pointer = ext_memory_read(g_Registers.pc++);
	MLOG(" ($%02X), Y[%02X]", pointer, g_Registers.y);
	//3
	uint pcl = ext_memory_read(pointer);
	//4.1 fetch high byte of effective address
	uint pch = ext_memory_read(TO_ZERO_PAGE(pointer + 1)) << 8;
	//4.2 add Y to low byte of effective address
	pcl += g_Registers.y;
	//5.1 read from effective address could be invalid
	ext_memory_read(TO_ZERO_PAGE(pcl) | pch);
	//5.2 fix high byte of effective address
	uint address = (pcl + pch) & 0xFFFF;
	//6.1 Do Operation
	switch (g_Registers.opCode)
	{
	case OPCODE_STA_IN_Y:
		cpu_sta();
		break;

	}
	//6.2
	MLOG(" A:$%04X <= $%02X", address, g_Registers.byteLatch);
	ext_memory_write(address, g_Registers.byteLatch);
}
