static inline void memory_implied_or_accumulator()
{
	ext_memory_read(g_Registers.pc);
	switch (g_Registers.opCode)
	{
	case OPCODE_CLC:
		cpu_clc();
		break;
	case OPCODE_CLD:
		cpu_cld();
		break;
	case OPCODE_CLI:
		cpu_cli();
		break;
	case OPCODE_CLV:
		cpu_clv();
		break;
	case OPCODE_DEX:
		cpu_dex();
		break;
	case OPCODE_DEY:
		cpu_dey();
		break;
	case OPCODE_INX:
		cpu_inx();
		break;
	case OPCODE_INY:
		cpu_iny();
		break;
	case OPCODE_NOP:
		cpu_nop();
		break;
	case OPCODE_SEC:
		cpu_sec();
		break;
	case OPCODE_SED:
		cpu_sed();
		break;
	case OPCODE_SEI:
		cpu_sei();
		break;
	case OPCODE_TAX:
		cpu_tax();
		break;
	case OPCODE_TAY:
		cpu_tay();
		break;
	case OPCODE_TSX:
		cpu_tsx();
		break;
	case OPCODE_TXA:
		cpu_txa();
		break;
	case OPCODE_TXS:
		cpu_txs();
		break;
	case OPCODE_TYA:
		cpu_tya();
		break;
	case OPCODE_ASL_A:
		internals::cpu_asl(g_Registers.a);
		break;
	case OPCODE_LSR_A:
		MLOG(" $%02X ->", g_Registers.a)
		internals::cpu_lsr(g_Registers.a);
		MLOG(" $%02X", g_Registers.a)
		break;
	case OPCODE_ROL_A:
		MLOG(" $%02X ->", g_Registers.a)
		internals::cpu_rol(g_Registers.a);
		MLOG(" $%02X", g_Registers.a)
		break;
	case OPCODE_ROR_A:
		MLOG(" $%02X ->", g_Registers.a)
		internals::cpu_ror(g_Registers.a);
		MLOG(" $%02X", g_Registers.a)
		break;
	}
}