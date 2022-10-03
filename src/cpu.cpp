#include "cpu.h"
#include "cpu_opcodes.h"
#include "cpu_registers.h"
#include "memory.h"
#include "ppu.h"
#include "apu.h"
#include "mapper.h"
#include "log.h"

#include <algorithm>


//#include "joystick.cpp"

void cpu_initialize(void* hInstance, void* hWnd)
{
	//joy1.create(hInstance, hWnd);
}

//tick!
void cpu_do_cycle()
{
	extern mapper_t* g_mapper;

	//this is where hijacking a interrupt can happen
	g_Registers.prev_nmi = g_Registers.nmi;
	//g_Registers.prev_irq = (IF_INTERRUPT() == false) ? g_Registers.irq : 0;

	//1 apu cycles per 2 cpu cycle
	apu_do_cycle();
	
	//3 cycles for every cpu cycle
	ppu_do_cycle();
	ppu_do_cycle();
	ppu_do_cycle();

	g_mapper->do_cpu_cycle();
}


void cpu_reset()
{
	g_Registers.tick_count = 0;
	g_Registers.delayed = delayed_i::empty;
	g_Registers.a = g_Registers.y = g_Registers.x = 0;
	g_Registers.irq = 0;
	g_Registers.status = IRQ_DISABLE_FLAG_MASK;
	g_Registers.stack = 0xFD;
	g_Registers.pc = (memory_main_read(RESETLO)) | (memory_main_read(RESETHI) << 8);
	apu_reset();
	ppu_reset();
}


#if 0
void cpu_execute() {
	g_Registers.extraCycles = 0;
	g_Registers.bMidWrite = false;
	switch (g_Registers.opCode & 0xFF) {
	case OPCODE_BRK:
		ext_memory_read(g_Registers.pc + 1);
		cpu_brk();
		g_Registers.cycles += 7;
		break;
	case OPCODE_ORA_IN_X:
		cpu_ora();
		g_Registers.pc += 2;
		g_Registers.cycles += 6;
		break;
	case 2: break;
	case OPCODE_SLO_IN_X: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_slo();
		g_Registers.pc += 2;
		g_Registers.cycles += 8;
		break;
	case 4: break;
	case OPCODE_ORA_ZP:
		cpu_ora();
		g_Registers.pc += 2;
		g_Registers.cycles += 3;
		break;
	case OPCODE_ASL_ZP:
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_asl();
		g_Registers.pc += 2;
		g_Registers.cycles += 5;
		break;
	case OPCODE_SLO_ZP: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_slo();
		g_Registers.pc += 2;
		g_Registers.cycles += 5;
		break;
	case OPCODE_PHP:
		ext_memory_read(g_Registers.pc + 1);
		cpu_php();
		g_Registers.pc += 1;
		g_Registers.cycles += 3;
		break;
	case OPCODE_ORA_OP:
		cpu_ora();
		g_Registers.pc += 2;
		g_Registers.cycles += 2;
		break;
	case OPCODE_ASL_A:
		ext_memory_read(g_Registers.pc + 1);
		cpu_asl();
		g_Registers.pc += 1;
		g_Registers.cycles += 2;
		break;
	case 11: break;
	case 12: break;
	case OPCODE_ORA_AB:
		cpu_ora();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		break;
	case OPCODE_ASL_AB:
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_asl();
		g_Registers.pc += 3;
		g_Registers.cycles += 6;
		break;
	case OPCODE_SLO_AB: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_slo();
		g_Registers.pc += 3;
		g_Registers.cycles += 6;
		break;
	case OPCODE_BPL:
		cpu_bpl();
		g_Registers.cycles += 2;
		g_Registers.cycles += g_Registers.extraCycles;
		break;
	case OPCODE_ORA_IN_Y:
		cpu_ora();
		g_Registers.pc += 2;
		g_Registers.cycles += 5;
		g_Registers.cycles += g_Registers.memoryExtraCycles;
		break;
	case 18: break;
	case OPCODE_SLO_IN_Y: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_slo();
		g_Registers.pc += 2;
		g_Registers.cycles += 8;
		break;
	case 20: break;
	case OPCODE_ORA_ZP_X:
		cpu_ora();
		g_Registers.pc += 2;
		g_Registers.cycles += 4;
		break;
	case OPCODE_ASL_ZP_X:
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_asl();
		g_Registers.pc += 2;
		g_Registers.cycles += 6;
		break;
	case OPCODE_SLO_ZP_X: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_slo();
		g_Registers.pc += 2;
		g_Registers.cycles += 6;
		break;
	case OPCODE_CLC:
		ext_memory_read(g_Registers.pc + 1);
		cpu_clc();
		g_Registers.pc += 1;
		g_Registers.cycles += 2;
		break;
	case OPCODE_ORA_AB_Y:
		cpu_ora();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		g_Registers.cycles += g_Registers.extraCycles;
		g_Registers.cycles += g_Registers.memoryExtraCycles;
		break;
	case OPCODE_NOP_1A: 
		ext_memory_read(g_Registers.pc + 1);
		cpu_nop();
		g_Registers.pc += 1;
		g_Registers.cycles += 2;
		break;
	case OPCODE_SLO_AB_Y: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_slo();
		g_Registers.pc += 3;
		g_Registers.cycles += 7;
		break;
	case 28: break;
	case OPCODE_ORA_AB_X:
		cpu_ora();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		g_Registers.cycles += g_Registers.extraCycles;
		g_Registers.cycles += g_Registers.memoryExtraCycles;
		break;
	case OPCODE_ASL_AB_X:
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_asl();
		g_Registers.pc += 3;
		g_Registers.cycles += 7;
		break;
	case OPCODE_SLO_AB_X: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_slo();
		g_Registers.pc += 3;
		g_Registers.cycles += 7;
		break;
	case OPCODE_JSR_AB:
		cpu_jsr();
		g_Registers.cycles += 6;
		break;
	case OPCODE_AND_IN_X:
		cpu_and();
		g_Registers.pc += 2;
		g_Registers.cycles += 6;
		break;
	case 34: break;
	case OPCODE_RLA_IN_X: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_rla();
		g_Registers.pc += 2;
		g_Registers.cycles += 8;
		break;
	case OPCODE_BIT_ZP:
		cpu_bit();
		g_Registers.pc += 2;
		g_Registers.cycles += 3;
		break;
	case OPCODE_AND_ZP:
		cpu_and();
		g_Registers.pc += 2;
		g_Registers.cycles += 3;
		break;
	case OPCODE_ROL_ZP:
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_rol();
		g_Registers.pc += 2;
		g_Registers.cycles += 5;
		break;
	case OPCODE_RLA_ZP: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_rla();
		g_Registers.pc += 2;
		g_Registers.cycles += 5;
		break;
	case OPCODE_PLP:
		ext_memory_read(g_Registers.pc + 1);
		cpu_plp();
		g_Registers.pc += 1;
		g_Registers.cycles += 4;
		break;
	case OPCODE_AND_OP:
		cpu_and();
		g_Registers.pc += 2;
		g_Registers.cycles += 2;
		break;
	case OPCODE_ROL_A:
		ext_memory_read(g_Registers.pc + 1);
		cpu_rol();
		g_Registers.pc += 1;
		g_Registers.cycles += 2;
		break;
	case 43: break;
	case OPCODE_BIT_AB:
		cpu_bit();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		break;
	case OPCODE_AND_AB:
		cpu_and();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		break;
	case OPCODE_ROL_AB:
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_rol();
		g_Registers.pc += 3;
		g_Registers.cycles += 6;
		break;
	case OPCODE_RLA_AB: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_rla();
		g_Registers.pc += 3;
		g_Registers.cycles += 6;
		break;
	case OPCODE_BMI:
		cpu_bmi();
		g_Registers.cycles += 2;
		g_Registers.cycles += g_Registers.extraCycles;
		break;
	case OPCODE_AND_IN_Y:
		cpu_and();
		g_Registers.pc += 2;
		g_Registers.cycles += 5;
		g_Registers.cycles += g_Registers.memoryExtraCycles;
		break;
	case 50: break;
	case OPCODE_RLA_IN_Y: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_rla();
		g_Registers.pc += 2;
		g_Registers.cycles += 8;
		break;
	case 52: break;
	case OPCODE_AND_ZP_X:
		cpu_and();
		g_Registers.pc += 2;
		g_Registers.cycles += 4;
		break;
	case OPCODE_ROL_ZP_X:
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_rol();
		g_Registers.pc += 2;
		g_Registers.cycles += 6;
		break;
	case OPCODE_RLA_ZP_X: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_rla();
		g_Registers.pc += 2;
		g_Registers.cycles += 6;
		break;
	case OPCODE_SEC:
		ext_memory_read(g_Registers.pc + 1);
		cpu_sec();
		g_Registers.pc += 1;
		g_Registers.cycles += 2;
		break;
	case OPCODE_AND_AB_Y:
		cpu_and();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		g_Registers.cycles += g_Registers.extraCycles;
		g_Registers.cycles += g_Registers.memoryExtraCycles;
		break;
	case OPCODE_NOP_3A: 
		ext_memory_read(g_Registers.pc + 1);
		cpu_nop();
		g_Registers.pc += 1;
		g_Registers.cycles += 2;
		break;
	case OPCODE_RLA_AB_Y: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_rla();
		g_Registers.pc += 3;
		g_Registers.cycles += 7;
		break;
	case 60: break;
	case OPCODE_AND_AB_X:
		cpu_and();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		g_Registers.cycles += g_Registers.extraCycles;
		g_Registers.cycles += g_Registers.memoryExtraCycles;
		break;
	case OPCODE_ROL_AB_X:
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_rol();
		g_Registers.pc += 3;
		g_Registers.cycles += 7;
		break;
	case OPCODE_RLA_AB_X: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_rla();
		g_Registers.pc += 3;
		g_Registers.cycles += 7;
		break;
	case OPCODE_RTI:
		ext_memory_read(g_Registers.pc + 1);
		cpu_rti();
		g_Registers.cycles += 6;
		break;
	case OPCODE_EOR_IN_X:
		cpu_eor();
		g_Registers.pc += 2;
		g_Registers.cycles += 6;
		break;
	case 66: break;
	case OPCODE_LSE_IN_X: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_lse();
		g_Registers.pc += 2;
		g_Registers.cycles += 8;
		break;
	case 68: break;
	case OPCODE_EOR_ZP:
		cpu_eor();
		g_Registers.pc += 2;
		g_Registers.cycles += 3;
		break;
	case OPCODE_LSR_ZP:
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_lsr();
		g_Registers.pc += 2;
		g_Registers.cycles += 5;
		break;
	case OPCODE_LSE_ZP: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_lse();
		g_Registers.pc += 2;
		g_Registers.cycles += 5;
		break;
	case OPCODE_PHA:
		ext_memory_read(g_Registers.pc + 1);
		cpu_pha();
		g_Registers.pc += 1;
		g_Registers.cycles += 3;
		break;
	case OPCODE_EOR_OP:
		cpu_eor();
		g_Registers.pc += 2;
		g_Registers.cycles += 2;
		break;
	case OPCODE_LSR_A:
		ext_memory_read(g_Registers.pc + 1);
		cpu_lsr();
		g_Registers.pc += 1;
		g_Registers.cycles += 2;
		break;
	case OPCODE_ALR_IMM: 
		cpu_alr();
		g_Registers.pc += 2;
		g_Registers.cycles += 2;
		break;
	case OPCODE_JMP_AB:
		cpu_jmp();
		g_Registers.cycles += 3;
		break;
	case OPCODE_EOR_AB:
		cpu_eor();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		break;
	case OPCODE_LSR_AB:
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_lsr();
		g_Registers.pc += 3;
		g_Registers.cycles += 6;
		break;
	case OPCODE_LSE_AB: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_lse();
		g_Registers.pc += 3;
		g_Registers.cycles += 6;
		break;
	case OPCODE_BVC:
		cpu_bvc();
		g_Registers.cycles += 2;
		g_Registers.cycles += g_Registers.extraCycles;
		break;
	case OPCODE_EOR_IN_Y:
		cpu_eor();
		g_Registers.pc += 2;
		g_Registers.cycles += 5;
		g_Registers.cycles += g_Registers.extraCycles;
		g_Registers.cycles += g_Registers.memoryExtraCycles;
		break;
	case 82: break;
	case OPCODE_LSE_IN_Y:
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_lse();
		g_Registers.pc += 2;
		g_Registers.cycles += 8;
		break;
	case 84: break;
	case OPCODE_EOR_ZP_X:
		cpu_eor();
		g_Registers.pc += 2;
		g_Registers.cycles += 4;
		break;
	case OPCODE_LSR_ZP_X:
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_lsr();
		g_Registers.pc += 2;
		g_Registers.cycles += 6;
		break;
	case OPCODE_LSE_ZP_X: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_lse();
		g_Registers.pc += 2;
		g_Registers.cycles += 6;
		break;
	case OPCODE_CLI:
		ext_memory_read(g_Registers.pc + 1);
		cpu_cli();
		g_Registers.pc += 1;
		g_Registers.cycles += 2;
		break;
	case OPCODE_EOR_AB_Y:
		cpu_eor();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		g_Registers.cycles += g_Registers.extraCycles;
		g_Registers.cycles += g_Registers.memoryExtraCycles;
		break;
	case OPCODE_NOP_5A: 
		ext_memory_read(g_Registers.pc + 1);
		cpu_nop();
		g_Registers.pc += 1;
		g_Registers.cycles += 2;
		break;
	case OPCODE_LSE_AB_Y: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_lse();
		g_Registers.pc += 3;
		g_Registers.cycles += 7;
		break;
	case 92: break;
	case OPCODE_EOR_AB_X:
		cpu_eor();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		g_Registers.cycles += g_Registers.extraCycles;
		g_Registers.cycles += g_Registers.memoryExtraCycles;
		break;
	case OPCODE_LSR_AB_X:
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_lsr();
		g_Registers.pc += 3;
		g_Registers.cycles += 7;
		break;
	case OPCODE_LSE_AB_X: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_lse();
		g_Registers.pc += 3;
		g_Registers.cycles += 7;
		break;
	case OPCODE_RTS:
		ext_memory_read(g_Registers.pc + 1);
		cpu_rts();
		g_Registers.cycles += 6;
		break;
	case OPCODE_ADC_IN_X:
		cpu_adc();
		g_Registers.pc += 2;
		g_Registers.cycles += 6;
		break;
	case 98: break;
	case OPCODE_RRA_IN_X: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_rra();
		g_Registers.pc += 2;
		g_Registers.cycles += 8;
		break;
	case 100: break;
	case OPCODE_ADC_ZP:
		cpu_adc();
		g_Registers.pc += 2;
		g_Registers.cycles += 3;
		break;
	case OPCODE_ROR_ZP:
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_ror();
		g_Registers.pc += 2;
		g_Registers.cycles += 5;
		break;
	case OPCODE_RRA_ZP: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_rra();
		g_Registers.pc += 2;
		g_Registers.cycles += 5;
		break;
	case OPCODE_PLA:
		ext_memory_read(g_Registers.pc + 1);
		cpu_pla();
		g_Registers.pc += 1;
		g_Registers.cycles += 4;
		break;
	case OPCODE_ADC_OP:
		cpu_adc();
		g_Registers.pc += 2;
		g_Registers.cycles += 2;
		break;
	case OPCODE_ROR_A:
		ext_memory_read(g_Registers.pc + 1);
		cpu_ror();
		g_Registers.pc += 1;
		g_Registers.cycles += 2;
		break;
	case OPCODE_ARR_IMM: 
		cpu_arr();
		g_Registers.pc += 2;
		g_Registers.cycles += 2;
		break;
	case OPCODE_JMP_IN:
		cpu_jmp();
		g_Registers.cycles += 5;
		break;
	case OPCODE_ADC_AB:
		cpu_adc();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		break;
	case OPCODE_ROR_AB:
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_ror();
		g_Registers.pc += 3;
		g_Registers.cycles += 6;
		break;
	case OPCODE_RRA_AB: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_rra();
		g_Registers.pc += 3;
		g_Registers.cycles += 6;
		break;
	case OPCODE_BVS:
		cpu_bvs();
		g_Registers.cycles += 2;
		g_Registers.cycles += g_Registers.extraCycles;
		break;
	case OPCODE_ADC_IN_Y:
		cpu_adc();
		g_Registers.pc += 2;
		g_Registers.cycles += 5;
		g_Registers.cycles += g_Registers.extraCycles;
		g_Registers.cycles += g_Registers.memoryExtraCycles;
		break;
	case 114: break;
	case OPCODE_RRA_IN_Y: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_rra();
		g_Registers.pc += 2;
		g_Registers.cycles += 8;
		break;
	case 116: break;
	case OPCODE_ADC_ZP_X:
		cpu_adc();
		g_Registers.pc += 2;
		g_Registers.cycles += 4;
		break;
	case OPCODE_ROR_ZP_X:
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_ror();
		g_Registers.pc += 2;
		g_Registers.cycles += 6;
		break;
	case OPCODE_RRA_ZP_X: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_rra();
		g_Registers.pc += 2;
		g_Registers.cycles += 6;
		break;
	case OPCODE_SEI:
		ext_memory_read(g_Registers.pc + 1);
		cpu_sei();
		g_Registers.pc += 1;
		g_Registers.cycles += 2;
		break;
	case OPCODE_ADC_AB_Y:
		cpu_adc();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		g_Registers.cycles += g_Registers.extraCycles;
		g_Registers.cycles += g_Registers.memoryExtraCycles;
		break;
	case OPCODE_NOP_7A: 
		ext_memory_read(g_Registers.pc + 1);
		cpu_nop();
		g_Registers.pc += 1;
		g_Registers.cycles += 2;
		break;
	case OPCODE_RRA_AB_Y: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_rra();
		g_Registers.pc += 3;
		g_Registers.cycles += 7;
		break;
	case 124: break;
	case OPCODE_ADC_AB_X:
		cpu_adc();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		g_Registers.cycles += g_Registers.extraCycles;
		g_Registers.cycles += g_Registers.memoryExtraCycles;
		break;
	case OPCODE_ROR_AB_X:
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_ror();
		g_Registers.pc += 3;
		g_Registers.cycles += 7;
		break;
	case OPCODE_RRA_AB_X: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_rra();
		g_Registers.pc += 3;
		g_Registers.cycles += 7;
		break;
	case 128: break;
	case OPCODE_STA_IN_X:
		cpu_sta();
		g_Registers.pc += 2;
		g_Registers.cycles += 6;
		break;
	case 130: break;
	case OPCODE_AXS_IN_X: 
		cpu_axs();
		g_Registers.pc += 2;
		g_Registers.cycles += 6;
		break;
	case OPCODE_STY_ZP:
		cpu_sty();
		g_Registers.pc += 2;
		g_Registers.cycles += 3;
		break;
	case OPCODE_STA_ZP:
		cpu_sta();
		g_Registers.pc += 2;
		g_Registers.cycles += 3;
		break;
	case OPCODE_STX_ZP:
		cpu_stx();
		g_Registers.pc += 2;
		g_Registers.cycles += 3;
		break;
	case OPCODE_AXS_ZP: 
		cpu_axs();
		g_Registers.pc += 2;
		g_Registers.cycles += 3;
		break;
	case OPCODE_DEY:
		ext_memory_read(g_Registers.pc + 1);
		cpu_dey();
		g_Registers.pc += 1;
		g_Registers.cycles += 2;
		break;
	case 137: break;
	case OPCODE_TXA:
		ext_memory_read(g_Registers.pc + 1);
		cpu_txa();
		g_Registers.pc += 1;
		g_Registers.cycles += 2;
		break;
	case OPCODE_XAA_IMM: 
		cpu_xaa();
		g_Registers.pc += 2;
		g_Registers.cycles += 2;
		break;
	case OPCODE_STY_AB:
		cpu_sty();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		break;
	case OPCODE_STA_AB:
		cpu_sta();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		break;
	case OPCODE_STX_AB:
		cpu_stx();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		break;
	case OPCODE_AXS_AB: 
		cpu_axs();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		break;
	case OPCODE_BCC:
		cpu_bcc();
		g_Registers.cycles += 2;
		g_Registers.cycles += g_Registers.extraCycles;
		break;
	case OPCODE_STA_IN_Y:
		cpu_sta();
		g_Registers.pc += 2;
		g_Registers.cycles += 6;
		break;
	case 146: break;
	case 147: break;
	case OPCODE_STY_ZP_X:
		cpu_sty();
		g_Registers.pc += 2;
		g_Registers.cycles += 4;
		break;
	case OPCODE_STA_ZP_X:
		cpu_sta();
		g_Registers.pc += 2;
		g_Registers.cycles += 4;
		break;
	case OPCODE_STX_ZP_Y:
		cpu_stx();
		g_Registers.pc += 2;
		g_Registers.cycles += 4;
		break;
	case OPCODE_AXS_ZP_Y: 
		cpu_axs();
		g_Registers.pc += 2;
		g_Registers.cycles += 4;
		break;
	case OPCODE_TYA:
		ext_memory_read(g_Registers.pc + 1);
		cpu_tya();
		g_Registers.pc += 1;
		g_Registers.cycles += 2;
		break;
	case OPCODE_STA_AB_Y:
		cpu_sta();
		g_Registers.pc += 3;
		g_Registers.cycles += 5;
		break;
	case OPCODE_TXS:
		ext_memory_read(g_Registers.pc + 1);
		cpu_txs();
		g_Registers.pc += 1;
		g_Registers.cycles += 2;
		break;
	case 155: break;
	case 156: break;
	case OPCODE_STA_AB_X:
		cpu_sta();
		g_Registers.pc += 3;
		g_Registers.cycles += 5;
		break;
	case 158: break;
	case 159: break;
	case OPCODE_LDY_OP:
		cpu_ldy();
		g_Registers.pc += 2;
		g_Registers.cycles += 2;
		break;
	case OPCODE_LDA_IN_X:
		cpu_lda();
		g_Registers.pc += 2;
		g_Registers.cycles += 6;
		break;
	case OPCODE_LDX_OP:
		cpu_ldx();
		g_Registers.pc += 2;
		g_Registers.cycles += 2;
		break;
	case OPCODE_LAX_IN_X: 
		cpu_lax();
		g_Registers.pc += 2;
		g_Registers.cycles += 6;
		break;
	case OPCODE_LDY_ZP:
		cpu_ldy();
		g_Registers.pc += 2;
		g_Registers.cycles += 3;
		break;
	case OPCODE_LDA_ZP:
		cpu_lda();
		g_Registers.pc += 2;
		g_Registers.cycles += 3;
		break;
	case OPCODE_LDX_ZP:
		cpu_ldx();
		g_Registers.pc += 2;
		g_Registers.cycles += 3;
		break;
	case OPCODE_LAX_ZP: 
		cpu_lax();
		g_Registers.pc += 2;
		g_Registers.cycles += 3;
		break;
	case OPCODE_TAY:
		ext_memory_read(g_Registers.pc + 1);
		cpu_tay();
		g_Registers.pc += 1;
		g_Registers.cycles += 2;
		break;
	case OPCODE_LDA_OP:
		cpu_lda();
		g_Registers.pc += 2;
		g_Registers.cycles += 2;
		break;
	case OPCODE_TAX:
		ext_memory_read(g_Registers.pc + 1);
		cpu_tax();
		g_Registers.pc += 1;
		g_Registers.cycles += 2;
		break;
	case OPCODE_OAL_IMM: 
		cpu_oal();
		g_Registers.pc += 2;
		g_Registers.cycles += 2;
		break;
	case OPCODE_LDY_AB:
		cpu_ldy();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		break;
	case OPCODE_LDA_AB:
		cpu_lda();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		break;
	case OPCODE_LDX_AB:
		cpu_ldx();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		break;
	case OPCODE_LAX_AB:
		cpu_lax();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		break;
	case OPCODE_BCS:
		cpu_bcs();
		g_Registers.cycles += 2;
		g_Registers.cycles += g_Registers.extraCycles;
		break;
	case OPCODE_LDA_IN_Y:
		cpu_lda();
		g_Registers.pc += 2;
		g_Registers.cycles += 5;
		g_Registers.cycles += g_Registers.extraCycles;
		g_Registers.cycles += g_Registers.memoryExtraCycles;
		break;
	case 178: break;
	case OPCODE_LAX_IN_Y: 
		cpu_lax();
		g_Registers.pc += 2;
		g_Registers.cycles += 5;
		g_Registers.cycles += g_Registers.extraCycles;
		g_Registers.cycles += g_Registers.memoryExtraCycles;
		break;
	case OPCODE_LDY_ZP_X:
		cpu_ldy();
		g_Registers.pc += 2;
		g_Registers.cycles += 4;
		break;
	case OPCODE_LDA_ZP_X:
		cpu_lda();
		g_Registers.pc += 2;
		g_Registers.cycles += 4;
		break;
	case OPCODE_LDX_ZP_Y:
		cpu_ldx();
		g_Registers.pc += 2;
		g_Registers.cycles += 4;
		break;
	case OPCODE_LAX_ZP_Y: 
		cpu_lax();
		g_Registers.pc += 2;
		g_Registers.cycles += 4;
		break;
	case OPCODE_CLV:
		ext_memory_read(g_Registers.pc + 1);
		cpu_clv();
		g_Registers.pc += 1;
		g_Registers.cycles += 2;
		break;
	case OPCODE_LDA_AB_Y:
		cpu_lda();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		g_Registers.cycles += g_Registers.extraCycles;
		g_Registers.cycles += g_Registers.memoryExtraCycles;
		break;
	case OPCODE_TSX:
		ext_memory_read(g_Registers.pc + 1);
		cpu_tsx();
		g_Registers.pc += 1;
		g_Registers.cycles += 2;
		break;
	case 187: break;
	case OPCODE_LDY_AB_X:
		cpu_ldy();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		g_Registers.cycles += g_Registers.extraCycles;
		g_Registers.cycles += g_Registers.memoryExtraCycles;
		break;
	case OPCODE_LDA_AB_X:
		cpu_lda();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		g_Registers.cycles += g_Registers.extraCycles;
		g_Registers.cycles += g_Registers.memoryExtraCycles;
		break;
	case OPCODE_LDX_AB_Y:
		cpu_ldx();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		g_Registers.cycles += g_Registers.extraCycles;
		g_Registers.cycles += g_Registers.memoryExtraCycles;
		break;
	case OPCODE_LAX_AB_Y: 
		cpu_lax();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		g_Registers.cycles += g_Registers.extraCycles;
		g_Registers.cycles += g_Registers.memoryExtraCycles;
		break;
	case OPCODE_CPY_OP:
		cpu_cpy();
		g_Registers.pc += 2;
		g_Registers.cycles += 2;
		break;
	case OPCODE_CMP_IN_X:
		cpu_cmp();
		g_Registers.pc += 2;
		g_Registers.cycles += 6;
		break;
	case 194: break;
	case OPCODE_DCP_IN_X: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_dcp();
		g_Registers.pc += 2;
		g_Registers.cycles += 8;
		break;
	case OPCODE_CPY_ZP:
		cpu_cpy();
		g_Registers.pc += 2;
		g_Registers.cycles += 3;
		break;
	case OPCODE_CMP_ZP:
		cpu_cmp();
		g_Registers.pc += 2;
		g_Registers.cycles += 3;
		break;
	case OPCODE_DEC_ZP:
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_dec();
		g_Registers.pc += 2;
		g_Registers.cycles += 5;
		break;
	case OPCODE_DCP_ZP: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_dcp();
		g_Registers.pc += 2;
		g_Registers.cycles += 5;
		break;
	case OPCODE_INY:
		ext_memory_read(g_Registers.pc + 1);
		cpu_iny();
		g_Registers.pc += 1;
		g_Registers.cycles += 2;
		break;
	case OPCODE_CMP_OP:
		cpu_cmp();
		g_Registers.pc += 2;
		g_Registers.cycles += 2;
		break;
	case OPCODE_DEX:
		ext_memory_read(g_Registers.pc + 1);
		cpu_dex();
		g_Registers.pc += 1;
		g_Registers.cycles += 2;
		break;
	case OPCODE_SAX_IMM: 
		cpu_sax();
		g_Registers.pc += 2;
		g_Registers.cycles += 2;
		break;
	case OPCODE_CPY_AB:
		cpu_cpy();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		break;
	case OPCODE_CMP_AB:
		cpu_cmp();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		break;
	case OPCODE_DEC_AB:
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_dec();
		g_Registers.pc += 3;
		g_Registers.cycles += 6;
		break;
	case OPCODE_DCP_AB: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_dcp();
		g_Registers.pc += 3;
		g_Registers.cycles += 6;
		break;
	case OPCODE_BNE:
		cpu_bne();
		g_Registers.cycles += 2;
		g_Registers.cycles += g_Registers.extraCycles;
		break;
	case OPCODE_CMP_IN_Y:
		cpu_cmp();
		g_Registers.pc += 2;
		g_Registers.cycles += 5;
		g_Registers.cycles += g_Registers.extraCycles;
		g_Registers.cycles += g_Registers.memoryExtraCycles;
		break;
	case 210: break;
	case OPCODE_DCP_IN_Y: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_dcp();
		g_Registers.pc += 2;
		g_Registers.cycles += 8;
		break;
	case 212: break;
	case OPCODE_CMP_ZP_X:
		cpu_cmp();
		g_Registers.pc += 2;
		g_Registers.cycles += 4;
		break;
	case OPCODE_DEC_ZP_X:
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_dec();
		g_Registers.pc += 2;
		g_Registers.cycles += 6;
		break;
	case OPCODE_DCP_ZP_X: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_dcp();
		g_Registers.pc += 2;
		g_Registers.cycles += 6;
		break;
	case OPCODE_CLD:
		ext_memory_read(g_Registers.pc + 1);
		cpu_cld();
		g_Registers.pc += 1;
		g_Registers.cycles += 2;
		break;
	case OPCODE_CMP_AB_Y:
		cpu_cmp();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		g_Registers.cycles += g_Registers.extraCycles;
		g_Registers.cycles += g_Registers.memoryExtraCycles;
		break;
	case OPCODE_NOP_DA: 
		ext_memory_read(g_Registers.pc + 1);
		cpu_nop();
		g_Registers.pc += 1;
		g_Registers.cycles += 2;
		break;
	case OPCODE_DCP_AB_Y: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_dcp();
		g_Registers.pc += 3;
		g_Registers.cycles += 7;
		break;
	case 220: break;
	case OPCODE_CMP_AB_X:
		cpu_cmp();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		g_Registers.cycles += g_Registers.extraCycles;
		g_Registers.cycles += g_Registers.memoryExtraCycles;
		break;
	case OPCODE_DEC_AB_X:
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_dec();
		g_Registers.pc += 3;
		g_Registers.cycles += 7;
		break;
	case OPCODE_DCP_AB_X: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_dcp();
		g_Registers.pc += 3;
		g_Registers.cycles += 7;
		break;
	case OPCODE_CPX_OP:
		cpu_cpx();
		g_Registers.pc += 2;
		g_Registers.cycles += 2;
		break;
	case OPCODE_SBC_IN_X:
		cpu_sbc();
		g_Registers.pc += 2;
		g_Registers.cycles += 6;
		break;
	case 226: break;
	case OPCODE_ISC_IN_X: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_isc();
		g_Registers.pc += 2;
		g_Registers.cycles += 8;
		break;
	case OPCODE_CPX_ZP:
		cpu_cpx();
		g_Registers.pc += 2;
		g_Registers.cycles += 3;
		break;
	case OPCODE_SBC_ZP:
		cpu_sbc();
		g_Registers.pc += 2;
		g_Registers.cycles += 3;
		break;
	case OPCODE_INC_ZP:
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_inc();
		g_Registers.pc += 2;
		g_Registers.cycles += 5;
		break;
	case OPCODE_ISC_ZP: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_isc();
		g_Registers.pc += 2;
		g_Registers.cycles += 5;
		break;
	case OPCODE_INX:
		ext_memory_read(g_Registers.pc + 1);
		cpu_inx();
		g_Registers.pc += 1;
		g_Registers.cycles += 2;
		break;
	case OPCODE_SBC_OP:
		cpu_sbc();
		g_Registers.pc += 2;
		g_Registers.cycles += 2;
		break;
	case OPCODE_NOP:
		ext_memory_read(g_Registers.pc + 1);
		cpu_nop();
		g_Registers.pc += 1;
		g_Registers.cycles += 2;
		break;
	case 235: break;
	case OPCODE_CPX_AB:
		cpu_cpx();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		break;
	case OPCODE_SBC_AB:
		cpu_sbc();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		break;
	case OPCODE_INC_AB:
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_inc();
		g_Registers.pc += 3;
		g_Registers.cycles += 6;
		break;
	case OPCODE_ISC_AB: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_isc();
		g_Registers.pc += 3;
		g_Registers.cycles += 6;
		break;
	case OPCODE_BEQ:
		cpu_beq();
		g_Registers.cycles += 2;
		g_Registers.cycles += g_Registers.extraCycles;
		break;
	case OPCODE_SBC_IN_Y:
		cpu_sbc();
		g_Registers.pc += 2;
		g_Registers.cycles += 5;
		g_Registers.cycles += g_Registers.memoryExtraCycles;
		break;
	case 242: break;
	case OPCODE_ISC_IN_Y: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_isc();
		g_Registers.pc += 2;
		g_Registers.cycles += 8;
		break;
	case 244: break;
	case OPCODE_SBC_ZP_X:
		cpu_sbc();
		g_Registers.pc += 2;
		g_Registers.cycles += 4;
		break;
	case OPCODE_INC_ZP_X:
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_inc();
		g_Registers.pc += 2;
		g_Registers.cycles += 6;
		break;
	case OPCODE_ISC_ZP_X: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_isc();
		g_Registers.pc += 2;
		g_Registers.cycles += 6;
		break;
	case OPCODE_SED:
		ext_memory_read(g_Registers.pc + 1);
		cpu_sed();
		g_Registers.pc += 1;
		g_Registers.cycles += 2;
		break;
	case OPCODE_SBC_AB_Y:
		cpu_sbc();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		g_Registers.cycles += g_Registers.extraCycles;
		g_Registers.cycles += g_Registers.memoryExtraCycles;
		break;
	case OPCODE_NOP_FA: 
		ext_memory_read(g_Registers.pc + 1);
		cpu_nop();
		g_Registers.pc += 1;
		g_Registers.cycles += 2;
		break;
	case OPCODE_ISC_AB_Y: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_isc();
		g_Registers.pc += 3;
		g_Registers.cycles += 7;
		break;
	case 252: break;
	case OPCODE_SBC_AB_X:
		cpu_sbc();
		g_Registers.pc += 3;
		g_Registers.cycles += 4;
		g_Registers.cycles += g_Registers.extraCycles;
		g_Registers.cycles += g_Registers.memoryExtraCycles;
		break;
	case OPCODE_INC_AB_X:
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_inc();
		g_Registers.pc += 3;
		g_Registers.cycles += 7;
		break;
	case OPCODE_ISC_AB_X: 
		g_Registers.bMidWrite = true;
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
		cpu_isc();
		g_Registers.pc += 3;
		g_Registers.cycles += 7;
		break;
	}

}

#endif