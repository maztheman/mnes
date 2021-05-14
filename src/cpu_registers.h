#ifndef __CPU_REGISTERS_H__
#define __CPU_REGISTERS_H__

typedef unsigned int uint;

struct Registers
{
	uint a,y,x;
	uint opCode;
	uint pc;
	uint cycles;
	uint extraCycles;
	uint byteLatch;
	uint addressLatch;
	uint status;
	uint stack;
	bool bMidWrite;
	uint memoryExtraCycles;

	bool nmi;
	bool prev_nmi;

	uint irq;
	uint prev_irq;
};

extern Registers g_Registers;

namespace irq {
	//irq flags
	enum irq {
		dpcm			= 0x01,
		apu_frame		= 0x02,
		mapper1			= 0x04,
		mapper2			= 0x08,
		fds				= 0x10,
	};

}

void set_dpcm_irq();
void set_apu_frame_irq();
void set_mapper1_irq();
void set_mapper2_irq();
void set_fds_irq();

void clear_dpcm_irq();
void clear_apu_frame_irq();
void clear_mapper1_irq();
void clear_mapper2_irq();
void clear_fds_irq();

bool get_dpcm_irq();
bool get_apu_frame_irq();
bool get_mapper1_irq();
bool get_mapper2_irq();
bool get_fds_irq();

bool is_carry();
bool IF_OVERFLOW();
bool IF_SIGN();
bool IF_ZERO();
bool IF_INTERRUPT();

void set_carry(bool bValue);
void SET_OVERFLOW(bool bValue);
void SET_SIGN(bool bValue);
void SET_ZERO(bool bValue);
void SET_INTERRUPT(bool bValue);
void SET_DECIMAL(bool bValue);
void set_nz(uint val);

#endif