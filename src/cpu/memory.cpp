#include "memory.h"
#include "memory_registers.h"
#include "cpu_registers.h"
#include "cpu_opcodes.h"

#include <common/Log.h>

#include <ppu/ppu.h>
#include <ppu/ppu_memory.h>
#include <ppu/ppu_registers.h>


#include "cpu.h"

#include <sound/apu.h>


static inline void cpu_ora();
static inline void cpu_eor();
static inline void cpu_and();
static inline void cpu_adc();
static inline void cpu_sbc();
static inline void cpu_cmp();
static inline void memory_inc_stack();
static inline void memory_dec_stack();
void memory_push_byte(uint nValue);
void memory_push_pc();


template<typename T>
static uint TO_ZERO_PAGE(T address)
{
	return address & 0xFF;
}



//opcodes 
#include "imp_opcodes.cpp"
#include "rwm_opcodes.cpp"
#include "r_opcodes.cpp"
#include "w_opcodes.cpp"
#include "stack_opcodes.cpp"
#include "rel_opcodes.cpp"

#include "imp_addressing.cpp"
#include "imm_addressing.cpp"
#include "r_addressing.cpp"
#include "w_addressing.cpp"
#include "jump_addressing.cpp"
#include "rel_addressing.cpp"


struct joystick_shift_reg_t
{
	uint reg = 0;
	uint shifted = 0;

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

using namespace mnes::memory;

struct memory_data_t
{
	joystick_shift_reg_t joy1;
	joystick_shift_reg_t joy2;
	std::unique_ptr<uint8_t[]> memory = std::make_unique<uint8_t[]>(MEMORY_SIZE);

	memory_t Memory()
	{
		return memory_t(memory.get(), MEMORY_SIZE);
	}
};

memory_data_t& MemoryData()
{
	static memory_data_t instance;
	return instance;
}

main_memory_t MainMemory()
{
	return main_memory_t(MemoryData().Memory().subspan(MAIN_MEMORY_OFFSET, MAIN_MEMORY_SIZE));
}

sram_memory_t SRam()
{
	return sram_memory_t(MemoryData().Memory().subspan(SRAM_OFFSET, SRAM_SIZE));
}

spr_ram_t SPRRam()
{
	return spr_ram_t(MemoryData().Memory().subspan(SPR_RAM_OFFSET, SPR_RAM_SIZE));
}

tmp_spr_ram_t TmpSPRRam()
{
	return tmp_spr_ram_t(MemoryData().Memory().subspan(TMP_SPR_RAM_OFFSET, TMP_SPR_RAM_SIZE));
}

#include "rwm_addressing.cpp"

void memory_intialize()
{
	GMemoryRegisters() = MemoryRegisters{};
	GRegisters().tick_count = 0;
	memset(MemoryData().memory.get(), 0, MEMORY_SIZE);
}

uint memory_pc_peek(uint address)
{
	return current_mapper()->read_memory(address);
}

uint memory_main_read(uint address)
{
	uint nMapperAnswer = current_mapper()->read_memory(address);

	if (address < 0x2000) {
		return MainMemory()[address & 0x7FF];
	}

	uint nRetval = 0;

	if (address < 0x4000) {
		address &= 7;
		switch(address)
		{
		case 0://W
			//nRetval = GMemoryRegisters().r2000;
			nRetval = GMemoryRegisters().ppu_latch_byte;
			break;
		case 1://W
			//nRetval =  GMemoryRegisters().r2001;
			nRetval = GMemoryRegisters().ppu_latch_byte;
			break;
		case 2://R
			nRetval =  GMemoryRegisters().r2002;
			/*
				Reading $2002 within a few PPU clocks of when VBL is set results in special-case behavior.
				Reading one PPU clock before reads it as clear and never sets the flag or generates NMI for that frame.
				Reading on the same PPU clock or one later reads it as set, clears it, and suppresses the NMI for that frame.
				Reading two or more PPU clocks before/after it's set behaves normally (reads flag's value, clears it, and doesn't affect NMI operation).
				This suppression behavior is due to the $2002 read pulling the NMI line back up too quickly after it drops (NMI is active low) for the CPU to see it.
				(CPU inputs like NMI are sampled each clock.)
			*/
			PPURegs().last_2002_read = ppu_get_cycle();
			clear_vblank();
			GMemoryRegisters().r2006Latch = false;
			break;
		case 3://W
			nRetval = GMemoryRegisters().ppu_latch_byte;
			break;
		case 4://RW
			if (GMemoryRegisters().oam_clear_reads) {
				nRetval = 0xFF;
			} else {
				nRetval = SPRRam()[GMemoryRegisters().r2003];
			}
			break;
		case 5://W
			nRetval = GMemoryRegisters().ppu_latch_byte;
			break;
		case 6://W
			nRetval = GMemoryRegisters().ppu_latch_byte;
			break;
		case 7://RW
			//read from ppu
			MLOG(" @ PPU Cycle %ld", ppu_get_current_scanline_cycle())
			uint ppuaddress = GMemoryRegisters().r2006;
			GMemoryRegisters().r2006 = ( GMemoryRegisters().r2006 + ((GMemoryRegisters().r2000 & 0x4)? 32 : 1)) & 0x7FFF;
			GMemoryRegisters().ppu_addr_bus = GMemoryRegisters().r2006;
			if (ppuaddress >= 0x3F00) {
				//returns value right away, but also sets the latch too
				GMemoryRegisters().r2006ByteLatch = ppu_memory_main_read(ppuaddress & 0x2FFF);
				nRetval = ppu_memory_main_read(ppuaddress);
			} else {
				nRetval = GMemoryRegisters().r2006ByteLatch;
				GMemoryRegisters().r2006ByteLatch = ppu_memory_main_read(ppuaddress);
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
			nRetval = (open_bus & 0xE0) | MemoryData().joy1.fetch();
			MemoryData().joy1.shift();
			break;
		case 0x17://rw - joystick #1 - hardcode to say its not connected
			nRetval = (open_bus & 0xE0) | MemoryData().joy2.fetch();
			MemoryData().joy2.shift();
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
	current_mapper()->write_memory(address, value);
	//g_pCurrentMapper->WriteMemory(address, value);

	if (address < 0x2000) {
		MainMemory()[address & 0x7FF] = TO_BYTE(value);
		return;
	}
	if (address < 0x4000) {
		
		address &= 0x7;
		switch(address)
		{
		case 0://W
			{
				bool bNmiEnabled = (GMemoryRegisters().r2000 & 0x80) == 0x80;
				GMemoryRegisters().r2000 = value;
				GMemoryRegisters().r2006Temp &= 0xF3FF;
				GMemoryRegisters().r2006Temp |= (value & 0x3) << 10;
				if (bNmiEnabled == false && (GMemoryRegisters().r2000 & 0x80) == 0x80 && is_vblank()) {
					printf("nmi set to true\n");
					GRegisters().nmi = true;
				}
				GMemoryRegisters().ppu_latch_byte = value;
			}
			break;
		case 1://W
			GMemoryRegisters().r2001  = value;
			GMemoryRegisters().ppu_latch_byte = value;
			break;
		case 2://R
			GMemoryRegisters().ppu_latch_byte = value;
			break;
		case 3://W
			GMemoryRegisters().ppu_latch_byte =  GMemoryRegisters().r2003 = value;
			break;
		case 4://RW
			SPRRam()[ GMemoryRegisters().r2003++ ] = TO_BYTE(value);
			GMemoryRegisters().r2003 &= 0xFF;
			break;
		case 5://W
			{
				if (GMemoryRegisters().r2006Latch) {//second write
					GMemoryRegisters().r2006Temp &= 0x8C1F;
					GMemoryRegisters().r2006Temp |= (value << 2) & 0x3E0;
					GMemoryRegisters().r2006Temp |= (value << 12) & 0x7000;
				} else {//first write
					GMemoryRegisters().TileXOffset = value & 0x7;
					GMemoryRegisters().r2006Temp &= 0xFFE0;//erase bits D0-D4
					GMemoryRegisters().r2006Temp |= (value >> 3) & 0x1F; //copy bit D3-D7 to D0-D4
				}
				GMemoryRegisters().r2006Latch = !GMemoryRegisters().r2006Latch;
				GMemoryRegisters().ppu_latch_byte = value;
				break;
			}
		case 6://W
			{
				if (GMemoryRegisters().r2006Latch) {//second write
					GMemoryRegisters().r2006Temp &= 0x3F00;//clear bottom, according to docs it says top 2 bits are cleared.  
					//in essense its masking 2006 to 0x3FFF, which is good because if you look inside write in 2007 it masks anyways to 0x3FFF;
					//side effect is Y-scroll gets 1 bit chopped off.
					//which is prolly why they use 2005 to scroll, since it is the scrolling register.
					GMemoryRegisters().r2006Temp |= value;//set bottom
					GMemoryRegisters().r2006 = GMemoryRegisters().r2006Temp;
					if (GMemoryRegisters().r2006 < 0x3F00) {
						MLOG(" @ PPU cycle %ld old $%04X", ppu_get_current_scanline_cycle(), GMemoryRegisters().ppu_addr_bus)
						GMemoryRegisters().ppu_addr_bus = GMemoryRegisters().r2006;
					}
				} else {//first write
					GMemoryRegisters().r2006Temp &= 0xFF;//clear top
					GMemoryRegisters().r2006Temp |= (value & 0x3F) << 8;//set top
				}
				GMemoryRegisters().r2006Latch = !GMemoryRegisters().r2006Latch;
				GMemoryRegisters().ppu_latch_byte = value;
				break;
			}
		case 7://RW
			{
				uint ppuAddress = GMemoryRegisters().r2006;
				GMemoryRegisters().r2006 = ( GMemoryRegisters().r2006 + ((GMemoryRegisters().r2000 & 0x4)? 32 : 1)) & 0x7FFF;
				if (GMemoryRegisters().r2006 < 0x3F00) {
					MLOG(" @ PPU cycle %ld old $%04X", ppu_get_current_scanline_cycle(), GMemoryRegisters().ppu_addr_bus)
					GMemoryRegisters().ppu_addr_bus = GMemoryRegisters().r2006;
				}
				ppu_memory_main_write(ppuAddress, value);
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
					MemoryData().joy1.set(cpu_get_joy1());
					MemoryData().joy2.set(cpu_get_joy2());
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

	if (address < 0x8000 && current_mapper()->m_bSaveRam) {//SRAM man..
		SRam()[ address & 0x1FFF ] = TO_BYTE(value);
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


static inline void memory_inc_stack()
{
	GRegisters().stack++;
	GRegisters().stack &= 0xFF;
}

static inline void memory_dec_stack()
{
	GRegisters().stack--;
	GRegisters().stack &= 0xFF;
}

void memory_push_byte(uint nValue)
{
	ext_memory_write(GRegisters().stack + 0x100, nValue);
	memory_dec_stack();
}


uint memory_pop_byte()
{
	memory_inc_stack();
	uint retval = ext_memory_read(GRegisters().stack + 0x100);
	return retval;
}

void memory_push_pc()
{
	ext_memory_write(GRegisters().stack + 0x100, (GRegisters().pc >> 8));
	memory_dec_stack();
	ext_memory_write(GRegisters().stack + 0x100, GRegisters().pc & 0xFF);
	memory_dec_stack();
}

void memory_pop_pc()
{
	memory_inc_stack();
	GRegisters().pc = ext_memory_read(GRegisters().stack + 0x100);
	memory_inc_stack();
	GRegisters().pc |= ext_memory_read(GRegisters().stack + 0x100) << 8;
}
/// <summary>
/// Only is done right after opcode is officially read
/// </summary>
static inline void check_for_hardware_irq()
{
	if (GRegisters().prev_nmi) {
		GRegisters().opCode = 0x00;
		GRegisters().actual_irq = doing_irq::nmi;
		MLOG("NMI -> ")
	} else if (IF_INTERRUPT() == false) {
		if (GRegisters().irq) {
			GRegisters().opCode = 0x00;
			GRegisters().actual_irq = doing_irq::irq;
			MLOG("IRQ -> ")
		} else {
			GRegisters().actual_irq = doing_irq::none;
			GRegisters().pc++;
		}
	} else {
		GRegisters().actual_irq = doing_irq::none;
		GRegisters().pc++;
	}
}

static inline void check_for_software_irq()
{
	if (GRegisters().actual_irq == doing_irq::none) {
		if (GRegisters().opCode == mnes::opcodes::OPCODE_BRK) {
			GRegisters().actual_irq = doing_irq::brk;
		}
	}
}

static inline void check_for_delayed_i_flag()
{
	if (GRegisters().delayed != delayed_i::empty) {
		SET_INTERRUPT(GRegisters().delayed == delayed_i::yes);
		GRegisters().delayed = delayed_i::empty;
	}
}

/// <summary>
/// Does a read, using 1 cycle
/// </summary>
static inline void memory_read_opcode()
{
	GRegisters().opCode = ext_memory_read(GRegisters().pc);
	
#ifdef USE_LOG	
	uint pc = GRegisters().pc;
#endif

	check_for_hardware_irq();
	check_for_software_irq();
	check_for_delayed_i_flag();

	MLOG("$%04X %02X %s", pc, GRegisters().opCode, OpCodes[GRegisters().opCode].sOpCode);
}


void memory_pc_process()
{
	memory_read_opcode();
	switch (GRegisters().opCode)
	{
		using namespace mnes::opcodes;
		//Read Instructions - Immediate
	case OPCODE_ADC_OP:
	case OPCODE_AND_OP:
	case OPCODE_CMP_OP:
	case OPCODE_CPX_OP:
	case OPCODE_CPY_OP:
	case OPCODE_EOR_OP:
	case OPCODE_LDA_OP:
	case OPCODE_LDX_OP:
	case OPCODE_LDY_OP:
	case OPCODE_ORA_OP:
	case OPCODE_SBC_OP:
	case OPCODE_SBC_IMM_EB:
	case OPCODE_ARR_IMM:
	case OPCODE_NOP_IMM:
	case OPCODE_NOP_IMM_82:
	case OPCODE_NOP_IMM_89:
	case OPCODE_NOP_IMM_C2:
	case OPCODE_NOP_IMM_E2:
	case OPCODE_ANC_IMM_0B:
	case OPCODE_ANC_IMM_2B:
	case OPCODE_ALR_IMM:
	case OPCODE_LAX_IMM:
	case OPCODE_AXS_IMM:
	case OPCODE_XAA_IMM:
		memory_r_immediate();
		break;
		//Read Instructions - Absolute
	case OPCODE_LDA_AB:
	case OPCODE_LDX_AB:
	case OPCODE_LDY_AB: 
	case OPCODE_EOR_AB:
	case OPCODE_AND_AB:
	case OPCODE_ORA_AB:
	case OPCODE_ADC_AB:
	case OPCODE_SBC_AB:
	case OPCODE_CMP_AB:
	case OPCODE_BIT_AB:
	case OPCODE_CPX_AB:
	case OPCODE_CPY_AB:
	case OPCODE_LAX_AB:
		memory_r_absolute();
		break;
		//Read Instructions - Zero Page
	case OPCODE_LDA_ZP:
	case OPCODE_LDX_ZP:
	case OPCODE_LDY_ZP:
	case OPCODE_EOR_ZP:
	case OPCODE_AND_ZP:
	case OPCODE_ORA_ZP:
	case OPCODE_ADC_ZP:
	case OPCODE_SBC_ZP:
	case OPCODE_CMP_ZP:
	case OPCODE_BIT_ZP:
	case OPCODE_LAX_ZP:
	case OPCODE_CPX_ZP:
	case OPCODE_CPY_ZP:
	case OPCODE_NOP_04_ZP:
	case OPCODE_NOP_44_ZP:
	case OPCODE_NOP_64_ZP:
		memory_r_zero_page();
		break;
		//Read Instructions - Zero Page Indexed
	case OPCODE_LDA_ZP_X:
	case OPCODE_LDY_ZP_X:
	case OPCODE_EOR_ZP_X:
	case OPCODE_AND_ZP_X:
	case OPCODE_ORA_ZP_X:
	case OPCODE_ADC_ZP_X:
	case OPCODE_SBC_ZP_X:
	case OPCODE_CMP_ZP_X:
	case OPCODE_NOP_14_ZP_X:
	case OPCODE_NOP_34_ZP_X:
	case OPCODE_NOP_54_ZP_X:
	case OPCODE_NOP_74_ZP_X:
	case OPCODE_NOP_D4_ZP_X:
	case OPCODE_NOP_F4_ZP_X:
		memory_r_zero_page_indexed_x();
		break;
	case OPCODE_LDX_ZP_Y:
	case OPCODE_LAX_ZP_Y:
		memory_r_zero_page_indexed_y();
		break;
		//Read Instructions - Absolute Indexed
	case OPCODE_LDA_AB_X:
	case OPCODE_LDY_AB_X:
	case OPCODE_EOR_AB_X:
	case OPCODE_AND_AB_X:
	case OPCODE_ORA_AB_X:
	case OPCODE_ADC_AB_X:
	case OPCODE_SBC_AB_X:
	case OPCODE_CMP_AB_X:
	case OPCODE_NOP_1C_AB_X:
	case OPCODE_NOP_3C_AB_X:
	case OPCODE_NOP_5C_AB_X:
	case OPCODE_NOP_7C_AB_X:
	case OPCODE_NOP_DC_AB_X:
	case OPCODE_NOP_FC_AB_X:
		memory_r_absolute_indexed_x();
		break;
	case OPCODE_LDA_AB_Y:
	case OPCODE_LDX_AB_Y:
	case OPCODE_EOR_AB_Y:
	case OPCODE_AND_AB_Y:
	case OPCODE_ORA_AB_Y:
	case OPCODE_ADC_AB_Y:
	case OPCODE_SBC_AB_Y:
	case OPCODE_CMP_AB_Y:
	case OPCODE_LAX_AB_Y:
		memory_r_absolute_indexed_y();
		break;
		//Read Instructions - Indexed Indirect
	case OPCODE_LDA_IN_X:
	case OPCODE_ORA_IN_X:
	case OPCODE_EOR_IN_X:
	case OPCODE_AND_IN_X:
	case OPCODE_ADC_IN_X:
	case OPCODE_CMP_IN_X:
	case OPCODE_SBC_IN_X:
	case OPCODE_LAX_IN_X:
		memory_r_indexed_indirect();
		break;
		//Read Instructions - Indirect Indexed
	case OPCODE_LDA_IN_Y:
	case OPCODE_EOR_IN_Y:
	case OPCODE_AND_IN_Y:
	case OPCODE_ORA_IN_Y:
	case OPCODE_ADC_IN_Y:
	case OPCODE_SBC_IN_Y:
	case OPCODE_CMP_IN_Y:
	case OPCODE_LAX_IN_Y:
		memory_r_indirect_indexed();
		break;
		//Read-Modify-Write instructions - Absolute
	case OPCODE_ASL_AB:
	case OPCODE_LSR_AB:
	case OPCODE_ROL_AB:
	case OPCODE_ROR_AB:
	case OPCODE_INC_AB:
	case OPCODE_DEC_AB:
	case OPCODE_SLO_AB:
	case OPCODE_LSE_AB:
	case OPCODE_RLA_AB:
	case OPCODE_RRA_AB:
	case OPCODE_ISC_AB:
	case OPCODE_DCP_AB:
		memory_rwm_absolute();
		break;
		//Read-Modify-Write instructions - Zero Page
	case OPCODE_ASL_ZP:
	case OPCODE_LSR_ZP:
	case OPCODE_ROL_ZP:
	case OPCODE_ROR_ZP:
	case OPCODE_INC_ZP:
	case OPCODE_DEC_ZP:
	case OPCODE_SLO_ZP:
	case OPCODE_LSE_ZP:
	case OPCODE_RLA_ZP:
	case OPCODE_RRA_ZP:
	case OPCODE_ISC_ZP:
	case OPCODE_DCP_ZP:
		memory_rwm_zero_page();
		break;
		//Read-Modify-Write instructions - Zero Page Indexed
	case OPCODE_ASL_ZP_X:
	case OPCODE_LSR_ZP_X:
	case OPCODE_ROL_ZP_X:
	case OPCODE_ROR_ZP_X:
	case OPCODE_INC_ZP_X:
	case OPCODE_DEC_ZP_X:
	case OPCODE_SLO_ZP_X:
	case OPCODE_LSE_ZP_X:
	case OPCODE_RLA_ZP_X:
	case OPCODE_RRA_ZP_X:
	case OPCODE_ISC_ZP_X:
	case OPCODE_DCP_ZP_X:
		memory_rwm_zero_page_indexed_x();
		break;
		//Read-Modify-Write instructions - Absolute Indexed
	case OPCODE_ASL_AB_X:
	case OPCODE_LSR_AB_X:
	case OPCODE_ROL_AB_X:
	case OPCODE_ROR_AB_X:
	case OPCODE_INC_AB_X:
	case OPCODE_DEC_AB_X:
	case OPCODE_SLO_AB_X:
	case OPCODE_LSE_AB_X:
	case OPCODE_RLA_AB_X:
	case OPCODE_RRA_AB_X:
	case OPCODE_ISC_AB_X:
	case OPCODE_DCP_AB_X:
		memory_rmw_absolute_indexed_x();
		break;
	case OPCODE_SLO_AB_Y:
	case OPCODE_LSE_AB_Y:
	case OPCODE_RLA_AB_Y:
	case OPCODE_RRA_AB_Y:
	case OPCODE_ISC_AB_Y:
	case OPCODE_DCP_AB_Y:
	case OPCODE_TAS_AB_Y:
	case OPCODE_AHX_AB_Y:
	case OPCODE_LAS_AB_Y:
		memory_rmw_absolute_indexed_y();
		break;
		//Read-Modify-Write instructions - Indexed indirect
	case OPCODE_SLO_IN_X:
	case OPCODE_LSE_IN_X:
	case OPCODE_RLA_IN_X:
	case OPCODE_RRA_IN_X:
	case OPCODE_ISC_IN_X:
	case OPCODE_DCP_IN_X:
		memory_rwm_indexed_indirect();
		break;
		//Read-Modify-Write instructions - Indirect Indexed
	case OPCODE_SLO_IN_Y:
	case OPCODE_LSE_IN_Y:
	case OPCODE_RLA_IN_Y:
	case OPCODE_RRA_IN_Y:
	case OPCODE_ISC_IN_Y:
	case OPCODE_DCP_IN_Y:
	case OPCODE_AHX_IN_Y:
		memory_rwm_indirect_indexed();
		break;
		//Write instructions - Absolute
	case OPCODE_STA_AB:
	case OPCODE_STX_AB:
	case OPCODE_STY_AB:
	//case OPCODE_SAX_AB:
		memory_w_absolute();
		break;
		//Write instructions - Zero Page
	case OPCODE_STA_ZP:
	case OPCODE_STX_ZP:
	case OPCODE_STY_ZP:
	case OPCODE_SAX_ZP:
		memory_w_zero_page();
		break;
		//Write instructions - Zero Page Indexed
	case OPCODE_STA_ZP_X:
	case OPCODE_STY_ZP_X:
		memory_w_zero_page_indexed_x();
		break;
	case OPCODE_STX_ZP_Y:
	case OPCODE_SAX_ZP_Y:
		memory_w_zero_page_indexed_y();
		break;
		//Write instructions - Absolute Indexed
	case OPCODE_STA_AB_X:
	//case OPCODE_STX_AB_X:
	//case OPCODE_STY_AB_X:
	//case OPCODE_SHA_AB_X:
	//case OPCODE_SHX_AB_X:
	case OPCODE_SHY_AB_X:
		memory_w_absolute_indexed_x();
		break;
	case OPCODE_STA_AB_Y:
	//case OPCODE_STX_AB_Y:
	//case OPCODE_STY_AB_Y:
	//case OPCODE_SHA_AB_Y:
	case OPCODE_SHX_AB_Y:
	//case OPCODE_SHY_AB_Y:

		memory_w_absolute_indexed_y();
		break;
		//Write instructions - Indexed indirect
	case OPCODE_STA_IN_X:
	//case OPCODE_SAX_IN_X:
		memory_w_indexed_indirect();
		break;
		//Write instructions - Indirect Indexed
	case OPCODE_STA_IN_Y:
	//case OPCODE_SHA_IN_Y:
		memory_w_indirect_indexed();
		break;
		//Stack instructions
	case OPCODE_BRK:
		cpu_brk();
		break;
	case OPCODE_RTI:
		cpu_rti();
		break;
	case OPCODE_RTS:
		cpu_rts();
		break;
	case OPCODE_PHA:
		MLOG(" | A:$%02X ->", GRegisters().a)
		cpu_pha();
		break;
	case OPCODE_PHP:
		cpu_php();
		break;
	case OPCODE_PLA:
		cpu_pla();
		MLOG(" | A:$%02X ->", GRegisters().a)
		break;
	case OPCODE_PLP:
		cpu_plp();
		break;
	case OPCODE_JSR_AB:
		cpu_jsr();
		break;
		//Branch Instructions
	case OPCODE_BCC:
	case OPCODE_BCS:
	case OPCODE_BNE:
	case OPCODE_BEQ:
	case OPCODE_BPL:
	case OPCODE_BMI:
	case OPCODE_BVC:
	case OPCODE_BVS:
		memory_rel();
		break;
		//Jumps
	case OPCODE_JMP_AB:
		memory_jmp_absolute();
		break;
	case OPCODE_JMP_IN:
		memory_jmp_absolute_indirect();
		break;
		//Implied or Accumulator
	case OPCODE_CLC:
	case OPCODE_CLD:
	case OPCODE_CLI:
	case OPCODE_CLV:
	case OPCODE_DEX:
	case OPCODE_DEY:
	case OPCODE_INX:
	case OPCODE_INY:
	case OPCODE_NOP:
	case OPCODE_SEC:
	case OPCODE_SED:
	case OPCODE_SEI:
	case OPCODE_TAX:
	case OPCODE_TAY:
	case OPCODE_TSX:
	case OPCODE_TXA:
	case OPCODE_TXS:
	case OPCODE_TYA:
	case OPCODE_ASL_A:
	case OPCODE_LSR_A:
	case OPCODE_ROR_A:
	case OPCODE_ROL_A:
	case OPCODE_NOP_1A:
	case OPCODE_NOP_3A:
	case OPCODE_NOP_5A:
	case OPCODE_NOP_7A:
	case OPCODE_NOP_DA:
	case OPCODE_NOP_FA:
		memory_implied_or_accumulator();
		break;
	default:
		MLOG(" Unknown opcode found %02X", GRegisters().opCode)
		break;
	}
	

	MLOG(" T:[$%016lX]\n", GRegisters().tick_count);

	GRegisters().tick_count++;
}
