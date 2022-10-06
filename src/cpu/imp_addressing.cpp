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
		MLOG(" | X:$%02X", g_Registers.x);
		break;
	case OPCODE_DEY:
		cpu_dey();
		MLOG(" | Y:$%02X", g_Registers.y);
		break;
	case OPCODE_INX:
		cpu_inx();
		MLOG(" | X:$%02X", g_Registers.x);
		break;
	case OPCODE_INY:
		cpu_iny();
		MLOG(" | Y:$%02X", g_Registers.y);
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
		MLOG(" | X:$%02X", g_Registers.x);
		break;
	case OPCODE_TAY:
		cpu_tay();
		MLOG(" | Y:$%02X", g_Registers.y);
		break;
	case OPCODE_TSX:
		cpu_tsx();
		MLOG(" | X:$%02X", g_Registers.x);
		break;
	case OPCODE_TXA:
		cpu_txa();
		MLOG(" | A:$%02X", g_Registers.a);
		break;
	case OPCODE_TXS:
		cpu_txs();
		MLOG(" | S:$%02X", g_Registers.stack);
		break;
	case OPCODE_TYA:
		cpu_tya();
		MLOG(" | A:$%02X", g_Registers.a);
		break;
	case OPCODE_ASL_A:
		MLOG(" | A:$%02X ->", g_Registers.a)
		internals::cpu_asl(g_Registers.a);
		MLOG(" A:$%02X", g_Registers.a);
		break;
	case OPCODE_LSR_A:
		MLOG(" | A:$%02X ->", g_Registers.a)
		internals::cpu_lsr(g_Registers.a);
		MLOG(" A:$%02X", g_Registers.a)
		break;
	case OPCODE_ROL_A:
		MLOG(" | A:$%02X ->", g_Registers.a)
		internals::cpu_rol(g_Registers.a);
		MLOG(" A:$%02X", g_Registers.a)
		break;
	case OPCODE_ROR_A:
		MLOG(" | A:$%02X ->", g_Registers.a)
		internals::cpu_ror(g_Registers.a);
		MLOG(" A:$%02X", g_Registers.a)
		break;
	}
}