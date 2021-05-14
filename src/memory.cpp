#include "memory.h"
#include "memory_registers.h"
#include "cpu_registers.h"
#include "cpu_opcodes.h"
#include "mapper.h"
#include "ppu.h"
#include "ppu_memory.h"
#include "ppu_registers.h"
#include "log.h"
#include "cpu.h"
#include "apu.h"

extern mapper_t* g_mapper;

struct joystick_shift_reg_t
{
	uint reg;
	uint shifted;

	void set(uint value) {
		reg = value;
		shifted = 8;
	}

	uint fetch() {
		if (shifted == 0) {
			return 1;
		}
		return reg & 1;
	}

	void shift() {
		reg &= 0xFE;
		reg >>= 1;
		if (shifted > 0) {
			shifted--;
		}
	}
};

joystick_shift_reg_t g_joy1 = {0};
joystick_shift_reg_t g_joy2 = {0};

uchar g_MainMemory[0x800] = {0};
uchar g_SRAM[0x2000] = {0};
uchar g_SPR_RAM[0x100] = {0};
uchar g_TMP_SPR_RAM[32] = {0};

#include "addressing_modes.cpp"

uint memory_main_read(uint address)
{
	uint nMapperAnswer = g_mapper->read_memory(address);

	if (address < 0x2000) {
		return g_MainMemory[address & 0x7FF];
	}

	uint nRetval = 0;

	if (address < 0x4000) {
		address &= 7;
		switch(address)
		{
		case 0://W
			//nRetval = g_MemoryRegisters.r2000;
			nRetval = g_MemoryRegisters.ppu_latch_byte;
			break;
		case 1://W
			//nRetval =  g_MemoryRegisters.r2001;
			nRetval = g_MemoryRegisters.ppu_latch_byte;
			break;
		case 2://R
			nRetval =  g_MemoryRegisters.r2002;
			g_PPURegisters.last_2002_read = ppu_get_cycle();
			clear_vblank();
			g_MemoryRegisters.r2006Latch = false;
			break;
		case 3://W
			nRetval = g_MemoryRegisters.ppu_latch_byte;
			break;
		case 4://RW
			if (g_MemoryRegisters.oam_clear_reads) {
				nRetval = 0xFF;
			} else {
				nRetval = g_SPR_RAM[g_MemoryRegisters.r2003];
			}
			break;
		case 5://W
			nRetval = g_MemoryRegisters.ppu_latch_byte;
			break;
		case 6://W
			nRetval = g_MemoryRegisters.ppu_latch_byte;
			break;
		case 7://RW
			//read from ppu
			uint ppuaddress = g_MemoryRegisters.r2006;
			g_MemoryRegisters.r2006 = ( g_MemoryRegisters.r2006 + ((g_MemoryRegisters.r2000 & 0x4)? 32 : 1)) & 0x7FFF;
			g_MemoryRegisters.ppu_addr_bus = g_MemoryRegisters.r2006;
			if (ppuaddress >= 0x3F00) {
				//returns value right away, but also sets the latch too
				g_MemoryRegisters.r2006ByteLatch = ppu_memory_main_read(ppuaddress & 0x2FFF);
				nRetval = ppu_memory_main_read(ppuaddress);
			} else {
				nRetval = g_MemoryRegisters.r2006ByteLatch;
				g_MemoryRegisters.r2006ByteLatch = ppu_memory_main_read(ppuaddress);
			}
			break;
		}

		return nRetval;
	}

	if (address < 0x6000) {
		const uint original = address;
		uint open_bus = original >> 8;
		address &= 0x1F;

		if (original == 0x4015) {
			return apu_memory_read(original);
		}
		//joypad
		switch(address)
		{
		case 0x14://w
		case 0x15://w
			//non readable
			nRetval = open_bus; //open bus !!
			break;
		case 0x16://rw - joystick #0 - hardcode to say its connected but no button pressed.
			nRetval = (open_bus & 0xE0) | g_joy1.fetch();
			g_joy1.shift();
			break;
		case 0x17://rw - joystick #1 - hardcode to say its not connected
			nRetval = (open_bus & 0xE0) | g_joy2.fetch();
			g_joy2.shift();
			break;
		default:
			nRetval = open_bus;
			break;
		}

		return nRetval;
	}

	//Sram is handled in mapper i guess...GL!

	return nMapperAnswer;
}

void memory_main_write(uint address, uint value)
{
	g_mapper->write_memory(address, value);
	//g_pCurrentMapper->WriteMemory(address, value);

	if (address < 0x2000) {
		g_MainMemory[address & 0x7FF] = value & 0xFF;
		return;
	}
	if (address < 0x4000) {
		
		address &= 0x7;
		switch(address)
		{
		case 0://W
			{
				bool bNmiEnabled = (g_MemoryRegisters.r2000 & 0x80) == 0x80;
				g_MemoryRegisters.r2000 = value;
				g_MemoryRegisters.r2006Temp &= 0xF3FF;
				g_MemoryRegisters.r2006Temp |= (value & 0x3) << 10;
				if (bNmiEnabled == false && (g_MemoryRegisters.r2000 & 0x80) == 0x80 && is_vblank()) {
					g_Registers.nmi = true;
				}
				g_MemoryRegisters.ppu_latch_byte = value;
			}
			break;
		case 1://W
			g_MemoryRegisters.r2001  = value;
			g_MemoryRegisters.ppu_latch_byte = value;
			break;
		case 2://R
			g_MemoryRegisters.ppu_latch_byte = value;
			break;
		case 3://W
			g_MemoryRegisters.ppu_latch_byte =  g_MemoryRegisters.r2003 = value;
			break;
		case 4://RW
			g_SPR_RAM[ g_MemoryRegisters.r2003++ ] = value;
			g_MemoryRegisters.r2003 &= 0xFF;
			break;
		case 5://W
			{
				if (g_MemoryRegisters.r2006Latch) {//second write
					g_MemoryRegisters.r2006Temp &= 0x8C1F;
					g_MemoryRegisters.r2006Temp |= (value << 2) & 0x3E0;
					g_MemoryRegisters.r2006Temp |= (value << 12) & 0x7000;
				} else {//first write
					g_MemoryRegisters.TileXOffset = value & 0x7;
					g_MemoryRegisters.r2006Temp &= 0xFFE0;//erase bits D0-D4
					g_MemoryRegisters.r2006Temp |= (value >> 3) & 0x1F; //copy bit D3-D7 to D0-D4
				}
				g_MemoryRegisters.r2006Latch = !g_MemoryRegisters.r2006Latch;
				g_MemoryRegisters.ppu_latch_byte = value;
				break;
			}
		case 6://W
			{
				if (g_MemoryRegisters.r2006Latch) {//second write
					g_MemoryRegisters.r2006Temp &= 0x3F00;//clear bottom, according to docs it says top 2 bits are cleared.  
					//in essense its masking 2006 to 0x3FFF, which is good because if you look inside write in 2007 it masks anyways to 0x3FFF;
					//side effect is Y-scroll gets 1 bit chopped off.
					//which is prolly why they use 2005 to scroll, since it is the scrolling register.
					g_MemoryRegisters.r2006Temp |= value;//set bottom
					g_MemoryRegisters.r2006 = g_MemoryRegisters.r2006Temp;
					g_MemoryRegisters.ppu_addr_bus = g_MemoryRegisters.r2006;
				} else {//first write
					g_MemoryRegisters.r2006Temp &= 0xFF;//clear top
					g_MemoryRegisters.r2006Temp |= (value & 0x3F) << 8;//set top
				}
				g_MemoryRegisters.r2006Latch = !g_MemoryRegisters.r2006Latch;
				g_MemoryRegisters.ppu_latch_byte = value;
				break;
			}
		case 7://RW
			{
				uint address = g_MemoryRegisters.r2006;
				g_MemoryRegisters.r2006 = ( g_MemoryRegisters.r2006 + ((g_MemoryRegisters.r2000 & 0x4)? 32 : 1)) & 0x7FFF;
				g_MemoryRegisters.ppu_addr_bus = g_MemoryRegisters.r2006;
				ppu_memory_main_write(address, value);
				break;
			}
		}
		return;
	}

	if (address < 0x6000) {
		address &= 0x1F;

		switch(address)
		{
		//Pulse 1
		case 0x00:
		case 0x01:
		case 0x02:
		case 0x03:
		//Pulse 2
		case 0x04:
		case 0x05:
		case 0x06:
		case 0x07:
		//Triangle
		case 0x08:
		case 0x0A:
		case 0x0B:
		//Noise
		case 0x0C:
		case 0x0E:
		case 0x0F:
		//DMC Channel
		case 0x10:
		case 0x11:
		case 0x12:
		case 0x13:
		//Control
		case 0x15:
		//Frame Counter
		case 0x17:
			apu_memory_write(address, value);
			break;
		case 0x14://w
		{
			uint nStartAddress = 0x100 * value;
			//keep the cpu and ppu in sync while dma is happening
			if (ppu_is_odd_cycle()) {
				cpu_do_cycle();
				cpu_do_cycle();
			} else {
				cpu_do_cycle();
			}
			for(uint i = 0; i < 256; i++) {
				ext_memory_write(0x2004, ext_memory_read(nStartAddress + i));
			}
			
			return;
		}
		case 0x16://rw
			{
				static int strobe = -1;
				if ( (value & 1) == 1) {
					strobe = 1;
				}
				if ((value & 1) == 0 && strobe == 1) {
					g_joy1.set(cpu_get_joy1());
					g_joy2.set(cpu_get_joy2());
					strobe = -1;
				}
			}
			break;
		default:
			//unknown...?
			break;
		}

		return;
	}

	if (address < 0x8000 && g_mapper->m_bSaveRam) {//SRAM man..
		g_SRAM[ address & 0x1FFF ] = value & 0xFF;
		return;
	}
}

uint ext_memory_read(uint address)
{
	cpu_do_cycle();
	return memory_main_read(address);
}

void ext_memory_write(uint address, uint value)
{
	cpu_do_cycle();
	memory_main_write(address, value);
}

void memory_push_byte(uint nValue)
{
	ext_memory_write(g_Registers.stack + 0x100, nValue);
	g_Registers.stack--;
	g_Registers.stack &= 0xFF;
}

uint memory_pop_byte()
{
	g_Registers.stack++;
	g_Registers.stack &= 0xFF;
	uint retval = ext_memory_read(g_Registers.stack + 0x100);
	return retval;
}

void memory_push_pc()
{
	ext_memory_write(g_Registers.stack + 0x100, (g_Registers.pc >> 8));
	g_Registers.stack--;
	g_Registers.stack &= 0xFF;
	ext_memory_write(g_Registers.stack + 0x100, g_Registers.pc & 0xFF);
	g_Registers.stack--;
	g_Registers.stack &= 0xFF;
}

void memory_pop_pc()
{
	g_Registers.stack++;
	g_Registers.stack &= 0xFF;
	g_Registers.pc = ext_memory_read(g_Registers.stack + 0x100);
	g_Registers.stack++;
	g_Registers.stack &= 0xFF;
	g_Registers.pc |= ext_memory_read(g_Registers.stack + 0x100) << 8;
}




void memory_read_latch()
{//this is where switches need to go
	bool bNeedsRead = false;
	//always calculate the address here
	//1 cycle
	g_Registers.opCode = ext_memory_read(g_Registers.pc);
	
	if (g_Registers.prev_nmi) {
		//g_Registers.nmi = false;
		g_Registers.opCode = 0x00;
		//cpu_execute_nmi();
	} else if (g_Registers.prev_irq) {//no idea how this will actually fucking work
		g_Registers.opCode = 0x00;
		//cpu_execute_irq();
	}

	switch( g_Registers.opCode & 0xFF) {
	case OPCODE_BRK:
		break;
	case OPCODE_ORA_IN_X:
		bNeedsRead = true;
		memory_indirect_x();
		break;
	case 2: break;
	case OPCODE_SLO_IN_X: 
		bNeedsRead = true;
		memory_indirect_x();
		break;
	case 4: break;
	case OPCODE_ORA_ZP:
		bNeedsRead = true;
		memory_zero_page();
		break;
	case OPCODE_ASL_ZP:
		bNeedsRead = true;
		memory_zero_page();
		break;
	case OPCODE_SLO_ZP: 
		bNeedsRead = true;
		memory_zero_page();
		break;
	case OPCODE_PHP:
		break;
	case OPCODE_ORA_OP:
		memory_immediate();
		break;
	case OPCODE_ASL_A:
		g_Registers.byteLatch = g_Registers.a;
		break;
	case 11: break;
	case 12: break;
	case OPCODE_ORA_AB:
		bNeedsRead = true;
		memory_absolute();
		break;
	case OPCODE_ASL_AB:
		bNeedsRead = true;
		memory_absolute();
		break;
	case OPCODE_SLO_AB: 
		bNeedsRead = true;
		memory_absolute();
		break;
	case OPCODE_BPL:
		memory_immediate();
		break;
	case OPCODE_ORA_IN_Y:
		bNeedsRead = true;
		memory_indirect_y();
		break;
	case 18: break;
	case OPCODE_SLO_IN_Y: 
		bNeedsRead = true;
		memory_indirect_y();
		break;
	case 20: break;
	case OPCODE_ORA_ZP_X:
		bNeedsRead = true;
		memory_zero_page_x();
		break;
	case OPCODE_ASL_ZP_X:
		bNeedsRead = true;
		memory_zero_page_x();
		break;
	case OPCODE_SLO_ZP_X: 
		bNeedsRead = true;
		memory_zero_page_x();
		break;
	case OPCODE_CLC:
		break;
	case OPCODE_ORA_AB_Y:
		bNeedsRead = true;
		memory_absolute_y();
		break;
	case OPCODE_NOP_1A: 
		break;
	case OPCODE_SLO_AB_Y: 
		bNeedsRead = true;
		memory_absolute_y();
		break;
	case 28: break;
	case OPCODE_ORA_AB_X:
		bNeedsRead = true;
		memory_absolute_x();
		break;
	case OPCODE_ASL_AB_X:
		bNeedsRead = true;
		memory_absolute_x();
		break;
	case OPCODE_SLO_AB_X: 
		bNeedsRead = true;
		memory_absolute_x();
		break;
	case OPCODE_JSR_AB:
		bNeedsRead = true;
		memory_absolute();
		break;
	case OPCODE_AND_IN_X:
		bNeedsRead = true;
		memory_indirect_x();
		break;
	case 34: break;
	case OPCODE_RLA_IN_X: 
		bNeedsRead = true;
		memory_indirect_x();
		break;
	case OPCODE_BIT_ZP:
		bNeedsRead = true;
		memory_zero_page();
		break;
	case OPCODE_AND_ZP:
		bNeedsRead = true;
		memory_zero_page();
		break;
	case OPCODE_ROL_ZP:
		bNeedsRead = true;
		memory_zero_page();
		break;
	case OPCODE_RLA_ZP: 
		bNeedsRead = true;
		memory_zero_page();
		break;
	case OPCODE_PLP:
		break;
	case OPCODE_AND_OP:
		memory_immediate();
		break;
	case OPCODE_ROL_A:
		g_Registers.byteLatch = g_Registers.a;
		break;
	case 43: break;
	case OPCODE_BIT_AB:
		bNeedsRead = true;
		memory_absolute();
		break;
	case OPCODE_AND_AB:
		bNeedsRead = true;
		memory_absolute();
		break;
	case OPCODE_ROL_AB:
		bNeedsRead = true;
		memory_absolute();
		break;
	case OPCODE_RLA_AB: 
		bNeedsRead = true;
		memory_absolute();
		break;
	case OPCODE_BMI:
		memory_immediate();
		break;
	case OPCODE_AND_IN_Y:
		bNeedsRead = true;
		memory_indirect_y();
		break;
	case 50: break;
	case OPCODE_RLA_IN_Y: 
		bNeedsRead = true;
		memory_indirect_y();
		break;
	case 52: break;
	case OPCODE_AND_ZP_X:
		bNeedsRead = true;
		memory_zero_page_x();
		break;
	case OPCODE_ROL_ZP_X:
		bNeedsRead = true;
		memory_zero_page_x();
		break;
	case OPCODE_RLA_ZP_X: 
		bNeedsRead = true;
		memory_zero_page_x();
		break;
	case OPCODE_SEC:
		break;
	case OPCODE_AND_AB_Y:
		bNeedsRead = true;
		memory_absolute_y();
		break;
	case OPCODE_NOP_3A: 
		break;
	case OPCODE_RLA_AB_Y: 
		bNeedsRead = true;
		memory_absolute_y();
		break;
	case 60: break;
	case OPCODE_AND_AB_X:
		bNeedsRead = true;
		memory_absolute_x();
		break;
	case OPCODE_ROL_AB_X:
		bNeedsRead = true;
		memory_absolute_x();
		break;
	case OPCODE_RLA_AB_X: 
		bNeedsRead = true;
		memory_absolute_x();
		break;
	case OPCODE_RTI:
		break;
	case OPCODE_EOR_IN_X:
		bNeedsRead = true;
		memory_indirect_x();
		break;
	case 66: break;
	case OPCODE_LSE_IN_X: 
		bNeedsRead = true;
		memory_indirect_x();
		break;
	case 68: break;
	case OPCODE_EOR_ZP:
		bNeedsRead = true;
		memory_zero_page();
		break;
	case OPCODE_LSR_ZP:
		bNeedsRead = true;
		memory_zero_page();
		break;
	case OPCODE_LSE_ZP: 
		bNeedsRead = true;
		memory_zero_page();
		break;
	case OPCODE_PHA:
		break;
	case OPCODE_EOR_OP:
		memory_immediate();
		break;
	case OPCODE_LSR_A:
		g_Registers.byteLatch = g_Registers.a;
		break;
	case OPCODE_ALR_IMM: 
		memory_immediate();
		break;
	case OPCODE_JMP_AB:
		memory_absolute();
		break;
	case OPCODE_EOR_AB:
		bNeedsRead = true;
		memory_absolute();
		break;
	case OPCODE_LSR_AB:
		bNeedsRead = true;
		memory_absolute();
		break;
	case OPCODE_LSE_AB: 
		bNeedsRead = true;
		memory_absolute();
		break;
	case OPCODE_BVC:
		memory_immediate();
		break;
	case OPCODE_EOR_IN_Y:
		bNeedsRead = true;
		memory_indirect_y();
		break;
	case 82: break;
	case OPCODE_LSE_IN_Y: 
		bNeedsRead = true;
		memory_indirect_y();
		break;
	case 84: break;
	case OPCODE_EOR_ZP_X:
		bNeedsRead = true;
		memory_zero_page_x();
		break;
	case OPCODE_LSR_ZP_X:
		bNeedsRead = true;
		memory_zero_page_x();
		break;
	case OPCODE_LSE_ZP_X: 
		bNeedsRead = true;
		memory_zero_page_x();
		break;
	case OPCODE_CLI:
		break;
	case OPCODE_EOR_AB_Y:
		bNeedsRead = true;
		memory_absolute_y();
		break;
	case OPCODE_NOP_5A: 
		break;
	case OPCODE_LSE_AB_Y: 
		bNeedsRead = true;
		memory_absolute_y();
		break;
	case 92: break;
	case OPCODE_EOR_AB_X:
		bNeedsRead = true;
		memory_absolute_x();
		break;
	case OPCODE_LSR_AB_X:
		bNeedsRead = true;
		memory_absolute_x();
		break;
	case OPCODE_LSE_AB_X: 
		bNeedsRead = true;
		memory_absolute_x();
		break;
	case OPCODE_RTS:
		break;
	case OPCODE_ADC_IN_X:
		bNeedsRead = true;
		memory_indirect_x();
		break;
	case 98: break;
	case OPCODE_RRA_IN_X: 
		bNeedsRead = true;
		memory_indirect_x();
		break;
	case 100: break;
	case OPCODE_ADC_ZP:
		bNeedsRead = true;
		memory_zero_page();
		break;
	case OPCODE_ROR_ZP:
		bNeedsRead = true;
		memory_zero_page();
		break;
	case OPCODE_RRA_ZP: 
		bNeedsRead = true;
		memory_zero_page();
		break;
	case OPCODE_PLA:
		break;
	case OPCODE_ADC_OP:
		memory_immediate();
		break;
	case OPCODE_ROR_A:
		g_Registers.byteLatch = g_Registers.a;
		break;
	case OPCODE_ARR_IMM: 
		memory_immediate();
		break;
	case OPCODE_JMP_IN:
		memory_absolute_indirect();
		break;
	case OPCODE_ADC_AB:
		bNeedsRead = true;
		memory_absolute();
		break;
	case OPCODE_ROR_AB:
		bNeedsRead = true;
		memory_absolute();
		break;
	case OPCODE_RRA_AB: 
		bNeedsRead = true;
		memory_absolute();
		break;
	case OPCODE_BVS:
		memory_immediate();
		break;
	case OPCODE_ADC_IN_Y:
		bNeedsRead = true;
		memory_indirect_y();
		break;
	case 114: break;
	case OPCODE_RRA_IN_Y: 
		bNeedsRead = true;
		memory_indirect_y();
		break;
	case 116: break;
	case OPCODE_ADC_ZP_X:
		bNeedsRead = true;
		memory_zero_page_x();
		break;
	case OPCODE_ROR_ZP_X:
		bNeedsRead = true;
		memory_zero_page_x();
		break;
	case OPCODE_RRA_ZP_X: 
		bNeedsRead = true;
		memory_zero_page_x();
		break;
	case OPCODE_SEI:
		break;
	case OPCODE_ADC_AB_Y:
		bNeedsRead = true;
		memory_absolute_y();
		break;
	case OPCODE_NOP_7A: 
		break;
	case OPCODE_RRA_AB_Y: 
		bNeedsRead = true;
		memory_absolute_y();
		break;
	case 124: break;
	case OPCODE_ADC_AB_X:
		bNeedsRead = true;
		memory_absolute_x();
		break;
	case OPCODE_ROR_AB_X:
		bNeedsRead = true;
		memory_absolute_x();
		break;
	case OPCODE_RRA_AB_X: 
		bNeedsRead = true;
		memory_absolute_x();
		break;
	case 128: break;
	case OPCODE_STA_IN_X:
		memory_indirect_x();
		break;
	case 130: break;
	case OPCODE_AXS_IN_X: 
		memory_indirect_x();
		break;
	case OPCODE_STY_ZP:
		memory_zero_page();
		break;
	case OPCODE_STA_ZP:
		memory_zero_page();
		break;
	case OPCODE_STX_ZP:
		memory_zero_page();
		break;
	case OPCODE_AXS_ZP: 
		memory_zero_page();
		break;
	case OPCODE_DEY:
		break;
	case 137: break;
	case OPCODE_TXA:
		break;
	case OPCODE_XAA_IMM: 
		memory_immediate();
		break;
	case OPCODE_STY_AB:
		memory_absolute();
		break;
	case OPCODE_STA_AB:
		memory_absolute();
		break;
	case OPCODE_STX_AB:
		memory_absolute();
		break;
	case OPCODE_AXS_AB: 
		memory_absolute();
		break;
	case OPCODE_BCC:
		memory_immediate();
		break;
	case OPCODE_STA_IN_Y:
		memory_indirect_y();
		break;
	case 146: break;
	case 147: break;
	case OPCODE_STY_ZP_X:
		memory_zero_page_x();
		break;
	case OPCODE_STA_ZP_X:
		memory_zero_page_x();
		break;
	case OPCODE_STX_ZP_Y:
		memory_zero_page_y();
		break;
	case OPCODE_AXS_ZP_Y: 
		memory_zero_page_y();
		break;
	case OPCODE_TYA:
		break;
	case OPCODE_STA_AB_Y:
		memory_absolute_y();
		break;
	case OPCODE_TXS:
		break;
	case 155: break;
	case 156: break;
	case OPCODE_STA_AB_X:
		memory_absolute_x();
		break;
	case 158: break;
	case 159: break;
	case OPCODE_LDY_OP:
		memory_immediate();
		break;
	case OPCODE_LDA_IN_X:
		bNeedsRead = true;
		memory_indirect_x();
		break;
	case OPCODE_LDX_OP:
		memory_immediate();
		break;
	case OPCODE_LAX_IN_X: 
		bNeedsRead = true;
		memory_indirect_x();
		break;
	case OPCODE_LDY_ZP:
		bNeedsRead = true;
		memory_zero_page();
		break;
	case OPCODE_LDA_ZP:
		bNeedsRead = true;
		memory_zero_page();
		break;
	case OPCODE_LDX_ZP:
		bNeedsRead = true;
		memory_zero_page();
		break;
	case OPCODE_LAX_ZP: 
		bNeedsRead = true;
		memory_zero_page();
		break;
	case OPCODE_TAY:
		break;
	case OPCODE_LDA_OP:
		memory_immediate();
		break;
	case OPCODE_TAX:
		break;
	case OPCODE_OAL_IMM: 
		memory_immediate();
		break;
	case OPCODE_LDY_AB:
		bNeedsRead = true;
		memory_absolute();
		break;
	case OPCODE_LDA_AB:
		bNeedsRead = true;
		memory_absolute();
		break;
	case OPCODE_LDX_AB:
		bNeedsRead = true;
		memory_absolute();
		break;
	case OPCODE_LAX_AB: 
		bNeedsRead = true;
		memory_absolute();
		break;
	case OPCODE_BCS:
		memory_immediate();
		break;
	case OPCODE_LDA_IN_Y:
		bNeedsRead = true;
		memory_indirect_y();
		break;
	case 178: break;
	case OPCODE_LAX_IN_Y: 
		bNeedsRead = true;
		memory_indirect_y();
		break;
	case OPCODE_LDY_ZP_X:
		bNeedsRead = true;
		memory_zero_page_x();
		break;
	case OPCODE_LDA_ZP_X:
		bNeedsRead = true;
		memory_zero_page_x();
		break;
	case OPCODE_LDX_ZP_Y:
		bNeedsRead = true;
		memory_zero_page_y();
		break;
	case OPCODE_LAX_ZP_Y: 
		bNeedsRead = true;
		memory_zero_page_y();
		break;
	case OPCODE_CLV:
		break;
	case OPCODE_LDA_AB_Y:
		bNeedsRead = true;
		memory_absolute_y();
		break;
	case OPCODE_TSX:
		break;
	case 187: break;
	case OPCODE_LDY_AB_X:
		bNeedsRead = true;
		memory_absolute_x();
		break;
	case OPCODE_LDA_AB_X:
		bNeedsRead = true;
		memory_absolute_x();
		break;
	case OPCODE_LDX_AB_Y:
		bNeedsRead = true;
		memory_absolute_y();
		break;
	case OPCODE_LAX_AB_Y: 
		bNeedsRead = true;
		memory_absolute_y();
		break;
	case OPCODE_CPY_OP:
		memory_immediate();
		break;
	case OPCODE_CMP_IN_X:
		bNeedsRead = true;
		memory_indirect_x();
		break;
	case 194: break;
	case OPCODE_DCP_IN_X: 
		bNeedsRead = true;
		memory_indirect_x();
		break;
	case OPCODE_CPY_ZP:
		bNeedsRead = true;
		memory_zero_page();
		break;
	case OPCODE_CMP_ZP:
		bNeedsRead = true;
		memory_zero_page();
		break;
	case OPCODE_DEC_ZP:
		bNeedsRead = true;
		memory_zero_page();
		break;
	case OPCODE_DCP_ZP: 
		bNeedsRead = true;
		memory_zero_page();
		break;
	case OPCODE_INY:
		break;
	case OPCODE_CMP_OP:
		memory_immediate();
		break;
	case OPCODE_DEX:
		break;
	case OPCODE_SAX_IMM: 
		memory_immediate();
		break;
	case OPCODE_CPY_AB:
		bNeedsRead = true;
		memory_absolute();
		break;
	case OPCODE_CMP_AB:
		bNeedsRead = true;
		memory_absolute();
		break;
	case OPCODE_DEC_AB:
		bNeedsRead = true;
		memory_absolute();
		break;
	case OPCODE_DCP_AB:
		bNeedsRead = true;
		memory_absolute();
		break;
	case OPCODE_BNE:
		memory_immediate();
		break;
	case OPCODE_CMP_IN_Y:
		bNeedsRead = true;
		memory_indirect_y();
		break;
	case 210: break;
	case OPCODE_DCP_IN_Y: 
		bNeedsRead = true;
		memory_indirect_y();
		break;
	case 212: break;
	case OPCODE_CMP_ZP_X:
		bNeedsRead = true;
		memory_zero_page_x();
		break;
	case OPCODE_DEC_ZP_X:
		bNeedsRead = true;
		memory_zero_page_x();
		break;
	case OPCODE_DCP_ZP_X: 
		bNeedsRead = true;
		memory_zero_page_x();
		break;
	case OPCODE_CLD:
		break;
	case OPCODE_CMP_AB_Y:
		bNeedsRead = true;
		memory_absolute_y();
		break;
	case OPCODE_NOP_DA:
		break;
	case OPCODE_DCP_AB_Y: 
		bNeedsRead = true;
		memory_absolute_y();
		break;
	case 220: break;
	case OPCODE_CMP_AB_X:
		bNeedsRead = true;
		memory_absolute_x();
		break;
	case OPCODE_DEC_AB_X:
		bNeedsRead = true;
		memory_absolute_x();
		break;
	case OPCODE_DCP_AB_X: 
		bNeedsRead = true;
		memory_absolute_x();
		break;
	case OPCODE_CPX_OP:
		memory_immediate();
		break;
	case OPCODE_SBC_IN_X:
		bNeedsRead = true;
		memory_indirect_x();
		break;
	case 226: break;
	case OPCODE_ISC_IN_X: 
		bNeedsRead = true;
		memory_indirect_x();
		break;
	case OPCODE_CPX_ZP:
		bNeedsRead = true;
		memory_zero_page();
		break;
	case OPCODE_SBC_ZP:
		bNeedsRead = true;
		memory_zero_page();
		break;
	case OPCODE_INC_ZP:
		bNeedsRead = true;
		memory_zero_page();
		break;
	case OPCODE_ISC_ZP: 
		bNeedsRead = true;
		memory_zero_page();
		break;
	case OPCODE_INX:
		break;
	case OPCODE_SBC_OP:
		memory_immediate();
		break;
	case OPCODE_NOP:
		break;
	case 235: break;
	case OPCODE_CPX_AB:
		bNeedsRead = true;
		memory_absolute();
		break;
	case OPCODE_SBC_AB:
		bNeedsRead = true;
		memory_absolute();
		break;
	case OPCODE_INC_AB:
		bNeedsRead = true;
		memory_absolute();
		break;
	case OPCODE_ISC_AB: 
		bNeedsRead = true;
		memory_absolute();
		break;
	case OPCODE_BEQ:
		memory_immediate();
		break;
	case OPCODE_SBC_IN_Y:
		bNeedsRead = true;
		memory_indirect_y();
		break;
	case 242: break;
	case OPCODE_ISC_IN_Y: 
		bNeedsRead = true;
		memory_indirect_y();
		break;
	case 244: break;
	case OPCODE_SBC_ZP_X:
		bNeedsRead = true;
		memory_zero_page_x();
		break;
	case OPCODE_INC_ZP_X:
		bNeedsRead = true;
		memory_zero_page_x();
		break;
	case OPCODE_ISC_ZP_X: 
		bNeedsRead = true;
		memory_zero_page_x();
		break;
	case OPCODE_SED:
		break;
	case OPCODE_SBC_AB_Y:
		bNeedsRead = true;
		memory_absolute_y();
		break;
	case OPCODE_NOP_FA: 
		break;
	case OPCODE_ISC_AB_Y: 
		bNeedsRead = true;
		memory_absolute_y();
		break;
	case 252: break;
	case OPCODE_SBC_AB_X:
		bNeedsRead = true;
		memory_absolute_x();
		break;
	case OPCODE_INC_AB_X:
		bNeedsRead = true;
		memory_absolute_x();
		break;
	case OPCODE_ISC_AB_X: 
		bNeedsRead = true;
		memory_absolute_x();
		break;
	}

	if (bNeedsRead) {
		g_Registers.byteLatch = ext_memory_read(g_Registers.addressLatch);
	}
}

void memory_write_latch()
{
	bool bNeedsWrite = false;

	switch( g_Registers.opCode & 0xFF) {
	case OPCODE_BRK:
		break;
	case OPCODE_ORA_IN_X:
		break;
	case 2: break;
	case OPCODE_SLO_IN_X: 
		bNeedsWrite = true;
		break;
	case 4: break;
	case OPCODE_ORA_ZP:
		break;
	case OPCODE_ASL_ZP:
		bNeedsWrite = true;
		break;
	case OPCODE_SLO_ZP: 
		bNeedsWrite = true;
		break;
	case OPCODE_PHP:
		break;
	case OPCODE_ORA_OP:
		break;
	case OPCODE_ASL_A:
		g_Registers.a = g_Registers.byteLatch;
		break;
	case 11: break;
	case 12: break;
	case OPCODE_ORA_AB:
		break;
	case OPCODE_ASL_AB:
		bNeedsWrite = true;
		break;
	case OPCODE_SLO_AB: 
		bNeedsWrite = true;
		break;
	case OPCODE_BPL:
		break;
	case OPCODE_ORA_IN_Y:
		bNeedsWrite = true;
		break;
	case 18: break;
	case OPCODE_SLO_IN_Y: 
		bNeedsWrite = true;
		break;
	case 20: break;
	case OPCODE_ORA_ZP_X:
		break;
	case OPCODE_ASL_ZP_X:
		bNeedsWrite = true;
		break;
	case OPCODE_SLO_ZP_X: 
		bNeedsWrite = true;
		break;
	case OPCODE_CLC:
		break;
	case OPCODE_ORA_AB_Y:
		break;
	case OPCODE_NOP_1A: 
		break;
	case OPCODE_SLO_AB_Y: 
		bNeedsWrite = true;
		break;
	case 28: break;
	case OPCODE_ORA_AB_X:
		break;
	case OPCODE_ASL_AB_X:
		bNeedsWrite = true;
		break;
	case OPCODE_SLO_AB_X: 
		bNeedsWrite = true;
		break;
	case OPCODE_JSR_AB:
		break;
	case OPCODE_AND_IN_X:
		break;
	case 34: break;
	case OPCODE_RLA_IN_X: 
		bNeedsWrite = true;
		break;
	case OPCODE_BIT_ZP:
		break;
	case OPCODE_AND_ZP:
		break;
	case OPCODE_ROL_ZP:
		bNeedsWrite = true;
		break;
	case OPCODE_RLA_ZP: 
		bNeedsWrite = true;
		break;
	case OPCODE_PLP:
		break;
	case OPCODE_AND_OP:
		break;
	case OPCODE_ROL_A:
		g_Registers.a = g_Registers.byteLatch;
		break;
	case 43: break;
	case OPCODE_BIT_AB:
		break;
	case OPCODE_AND_AB:
		break;
	case OPCODE_ROL_AB:
		bNeedsWrite = true;
		break;
	case OPCODE_RLA_AB: 
		bNeedsWrite = true;
		break;
	case OPCODE_BMI:
		break;
	case OPCODE_AND_IN_Y:
		break;
	case 50: break;
	case OPCODE_RLA_IN_Y: 
		bNeedsWrite = true;
		break;
	case 52: break;
	case OPCODE_AND_ZP_X:
		break;
	case OPCODE_ROL_ZP_X:
		bNeedsWrite = true;
		break;
	case OPCODE_RLA_ZP_X: 
		bNeedsWrite = true;
		break;
	case OPCODE_SEC:
		break;
	case OPCODE_AND_AB_Y:
		break;
	case OPCODE_NOP_3A: 
		break;
	case OPCODE_RLA_AB_Y: 
		bNeedsWrite = true;
		break;
	case 60: break;
	case OPCODE_AND_AB_X:
		break;
	case OPCODE_ROL_AB_X:
		bNeedsWrite = true;
		break;
	case OPCODE_RLA_AB_X: 
		bNeedsWrite = true;
		break;
	case OPCODE_RTI:
		break;
	case OPCODE_EOR_IN_X:
		break;
	case 66: break;
	case OPCODE_LSE_IN_X: 
		bNeedsWrite = true;
		break;
	case 68: break;
	case OPCODE_EOR_ZP:
		break;
	case OPCODE_LSR_ZP:
		bNeedsWrite = true;
		break;
	case OPCODE_LSE_ZP: 
		bNeedsWrite = true;
		break;
	case OPCODE_PHA:
		break;
	case OPCODE_EOR_OP:
		break;
	case OPCODE_LSR_A:
		g_Registers.a = g_Registers.byteLatch;
		break;
	case OPCODE_ALR_IMM: 
		break;
	case OPCODE_JMP_AB:
		break;
	case OPCODE_EOR_AB:
		break;
	case OPCODE_LSR_AB:
		bNeedsWrite = true;
		break;
	case OPCODE_LSE_AB: 
		bNeedsWrite = true;
		break;
	case OPCODE_BVC:
		break;
	case OPCODE_EOR_IN_Y:
		break;
	case 82: break;
	case OPCODE_LSE_IN_Y: 
		bNeedsWrite = true;
		break;
	case 84: break;
	case OPCODE_EOR_ZP_X:
		break;
	case OPCODE_LSR_ZP_X:
		bNeedsWrite = true;
		break;
	case OPCODE_LSE_ZP_X: 
		bNeedsWrite = true;
		break;
	case OPCODE_CLI:
		break;
	case OPCODE_EOR_AB_Y:
		break;
	case OPCODE_NOP_5A: 
		break;
	case OPCODE_LSE_AB_Y: 
		bNeedsWrite = true;
		break;
	case 92: break;
	case OPCODE_EOR_AB_X:
		break;
	case OPCODE_LSR_AB_X:
		bNeedsWrite = true;
		break;
	case OPCODE_LSE_AB_X: 
		bNeedsWrite = true;
		break;
	case OPCODE_RTS:
		break;
	case OPCODE_ADC_IN_X:
		break;
	case 98: break;
	case OPCODE_RRA_IN_X: 
		bNeedsWrite = true;
		break;
	case 100: break;
	case OPCODE_ADC_ZP:
		break;
	case OPCODE_ROR_ZP:
		bNeedsWrite = true;
		break;
	case OPCODE_RRA_ZP: 
		bNeedsWrite = true;
		break;
	case OPCODE_PLA:
		break;
	case OPCODE_ADC_OP:
		break;
	case OPCODE_ROR_A:
		g_Registers.a = g_Registers.byteLatch;
		break;
	case OPCODE_ARR_IMM: 
		break;
	case OPCODE_JMP_IN:
		break;
	case OPCODE_ADC_AB:
		break;
	case OPCODE_ROR_AB:
		bNeedsWrite = true;
		break;
	case OPCODE_RRA_AB: 
		bNeedsWrite = true;
		break;
	case OPCODE_BVS:
		break;
	case OPCODE_ADC_IN_Y:
		break;
	case 114: break;
	case OPCODE_RRA_IN_Y: 
		bNeedsWrite = true;
		break;
	case 116: break;
	case OPCODE_ADC_ZP_X:
		break;
	case OPCODE_ROR_ZP_X:
		bNeedsWrite = true;
		break;
	case OPCODE_RRA_ZP_X: 
		bNeedsWrite = true;
		break;
	case OPCODE_SEI:
		break;
	case OPCODE_ADC_AB_Y:
		break;
	case OPCODE_NOP_7A: 
		break;
	case OPCODE_RRA_AB_Y: 
		bNeedsWrite = true;
		break;
	case 124: break;
	case OPCODE_ADC_AB_X:
		break;
	case OPCODE_ROR_AB_X:
		bNeedsWrite = true;
		break;
	case OPCODE_RRA_AB_X: 
		bNeedsWrite = true;
		break;
	case 128: break;
	case OPCODE_STA_IN_X:
		bNeedsWrite = true;
		break;
	case 130: break;
	case OPCODE_AXS_IN_X: 
		bNeedsWrite = true;
		break;
	case OPCODE_STY_ZP:
		bNeedsWrite = true;
		break;
	case OPCODE_STA_ZP:
		bNeedsWrite = true;
		break;
	case OPCODE_STX_ZP:
		bNeedsWrite = true;
		break;
	case OPCODE_AXS_ZP: 
		bNeedsWrite = true;
		break;
	case OPCODE_DEY:
		break;
	case 137: break;
	case OPCODE_TXA:
		break;
	case OPCODE_XAA_IMM: 
		break;
	case OPCODE_STY_AB:
		bNeedsWrite = true;
		break;
	case OPCODE_STA_AB:
		bNeedsWrite = true;
		break;
	case OPCODE_STX_AB:
		bNeedsWrite = true;
		break;
	case OPCODE_AXS_AB: 
		bNeedsWrite = true;
		break;
	case OPCODE_BCC:
		break;
	case OPCODE_STA_IN_Y:
		bNeedsWrite = true;
		break;
	case 146: break;
	case 147: break;
	case OPCODE_STY_ZP_X:
		bNeedsWrite = true;
		break;
	case OPCODE_STA_ZP_X:
		bNeedsWrite = true;
		break;
	case OPCODE_STX_ZP_Y:
		bNeedsWrite = true;
		break;
	case OPCODE_AXS_ZP_Y: 
		bNeedsWrite = true;
		break;
	case OPCODE_TYA:
		break;
	case OPCODE_STA_AB_Y:
		bNeedsWrite = true;
		break;
	case OPCODE_TXS:
		break;
	case 155: break;
	case 156: break;
	case OPCODE_STA_AB_X:
		bNeedsWrite = true;
		break;
	case 158: break;
	case 159: break;
	case OPCODE_LDY_OP:
		break;
	case OPCODE_LDA_IN_X:
		break;
	case OPCODE_LDX_OP:
		break;
	case OPCODE_LAX_IN_X:
		break;
	case OPCODE_LDY_ZP:
		break;
	case OPCODE_LDA_ZP:
		break;
	case OPCODE_LDX_ZP:
		break;
	case OPCODE_LAX_ZP: 
		break;
	case OPCODE_TAY:
		break;
	case OPCODE_LDA_OP:
		break;
	case OPCODE_TAX:
		break;
	case OPCODE_OAL_IMM: 
		break;
	case OPCODE_LDY_AB:
		break;
	case OPCODE_LDA_AB:
		break;
	case OPCODE_LDX_AB:
		break;
	case OPCODE_LAX_AB: 
		break;
	case OPCODE_BCS:
		break;
	case OPCODE_LDA_IN_Y:
		break;
	case 178: break;
	case OPCODE_LAX_IN_Y: 
		break;
	case OPCODE_LDY_ZP_X:
		break;
	case OPCODE_LDA_ZP_X:
		break;
	case OPCODE_LDX_ZP_Y:
		break;
	case OPCODE_LAX_ZP_Y: 
		break;
	case OPCODE_CLV:
		break;
	case OPCODE_LDA_AB_Y:
		break;
	case OPCODE_TSX:
		break;
	case 187: break;
	case OPCODE_LDY_AB_X:
		break;
	case OPCODE_LDA_AB_X:
		break;
	case OPCODE_LDX_AB_Y:
		break;
	case OPCODE_LAX_AB_Y:
		break;
	case OPCODE_CPY_OP:
		break;
	case OPCODE_CMP_IN_X:
		break;
	case 194: break;
	case OPCODE_DCP_IN_X: 
		bNeedsWrite = true;
		break;
	case OPCODE_CPY_ZP:
		break;
	case OPCODE_CMP_ZP:
		break;
	case OPCODE_DEC_ZP:
		bNeedsWrite = true;
		break;
	case OPCODE_DCP_ZP: 
		bNeedsWrite = true;
		break;
	case OPCODE_INY:
		break;
	case OPCODE_CMP_OP:
		break;
	case OPCODE_DEX:
		break;
	case OPCODE_SAX_IMM: 
		break;
	case OPCODE_CPY_AB:
		break;
	case OPCODE_CMP_AB:
		break;
	case OPCODE_DEC_AB:
		bNeedsWrite = true;
		break;
	case OPCODE_DCP_AB: 
		bNeedsWrite = true;
		break;
	case OPCODE_BNE:
		break;
	case OPCODE_CMP_IN_Y:
		break;
	case 210: break;
	case OPCODE_DCP_IN_Y: 
		bNeedsWrite = true;
		break;
	case 212: break;
	case OPCODE_CMP_ZP_X:
		break;
	case OPCODE_DEC_ZP_X:
		bNeedsWrite = true;
		break;
	case OPCODE_DCP_ZP_X: 
		bNeedsWrite = true;
		break;
	case OPCODE_CLD:
		break;
	case OPCODE_CMP_AB_Y:
		break;
	case OPCODE_NOP_DA: 
		break;
	case OPCODE_DCP_AB_Y: 
		bNeedsWrite = true;
		break;
	case 220: break;
	case OPCODE_CMP_AB_X:
		break;
	case OPCODE_DEC_AB_X:
		bNeedsWrite = true;
		break;
	case OPCODE_DCP_AB_X: 
		bNeedsWrite = true;
		break;
	case OPCODE_CPX_OP:
		break;
	case OPCODE_SBC_IN_X:
		break;
	case 226: break;
	case OPCODE_ISC_IN_X: 
		bNeedsWrite = true;
		break;
	case OPCODE_CPX_ZP:
		break;
	case OPCODE_SBC_ZP:
		break;
	case OPCODE_INC_ZP:
		bNeedsWrite = true;
		break;
	case OPCODE_ISC_ZP: 
		bNeedsWrite = true;
		break;
	case OPCODE_INX:
		break;
	case OPCODE_SBC_OP:
		break;
	case OPCODE_NOP:
		break;
	case 235: break;
	case OPCODE_CPX_AB:
		break;
	case OPCODE_SBC_AB:
		break;
	case OPCODE_INC_AB:
		bNeedsWrite = true;
		break;
	case OPCODE_ISC_AB: 
		bNeedsWrite = true;
		break;
	case OPCODE_BEQ:
		break;
	case OPCODE_SBC_IN_Y:
		break;
	case 242: break;
	case OPCODE_ISC_IN_Y: 
		bNeedsWrite = true;
		break;
	case 244: break;
	case OPCODE_SBC_ZP_X:
		break;
	case OPCODE_INC_ZP_X:
		bNeedsWrite = true;
		break;
	case OPCODE_ISC_ZP_X: 
		bNeedsWrite = true;
		break;
	case OPCODE_SED:
		break;
	case OPCODE_SBC_AB_Y:
		break;
	case OPCODE_NOP_FA: 
		break;
	case OPCODE_ISC_AB_Y: 
		bNeedsWrite = true;
		break;
	case 252: break;
	case OPCODE_SBC_AB_X:
		break;
	case OPCODE_INC_AB_X:
		bNeedsWrite = true;
		break;
	case OPCODE_ISC_AB_X: 
		bNeedsWrite = true;
		break;
	}

	if (bNeedsWrite) {
		ext_memory_write(g_Registers.addressLatch, g_Registers.byteLatch);
	}
}