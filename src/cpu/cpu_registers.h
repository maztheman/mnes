#pragma once

#include <cstdint>

typedef unsigned int uint;

enum class delayed_i {
	empty = -1,
	no = 0,
	yes = 1,
};

enum class doing_irq {
	none = 0,
	nmi,
	irq,
	brk
};

struct Registers
{
	uint a,y,x;
	uint opCode;
	uint pc;
	uint byteLatch;
	uint addressLatch;
	uint status;
	uint stack;
	bool nmi;
	bool prev_nmi;

	uint irq;
	delayed_i delayed;
	doing_irq actual_irq;
	uint64_t tick_count{ 0 };
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

constexpr uint32_t CARRY_FLAG_MASK			= 0x01;
constexpr uint32_t ZERO_FLAG_MASK			= 0x02;
constexpr uint32_t IRQ_DISABLE_FLAG_MASK	= 0x04;
constexpr uint32_t DECIMAL_FLAG_MASK		= 0x08;
constexpr uint32_t BFLAG_01_MASK			= 0x10;
constexpr uint32_t BFLAG_10_MASK			= 0x20;
constexpr uint32_t OVERFLOW_FLAG_MASK		= 0x40;
constexpr uint32_t NEGATIVE_FLAG_MASK		= 0x80;

//Used in PLP and RTI because these bits are ignored
constexpr uint32_t BFLAG_CLEAR_MASK			= 0xCF;