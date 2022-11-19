#pragma once

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
	uint32_t  a{0U},y{0U},x{0U};
	uint32_t  opCode{0U};
	uint32_t  pc{0U};
	uint32_t  byteLatch{0U};
	uint32_t  addressLatch{0U};
	uint32_t  status{0U};
	uint32_t  stack{0U};
	bool nmi{false};
	bool prev_nmi{false};
	uint32_t  irq{0U};
	delayed_i delayed{delayed_i::empty};
	doing_irq actual_irq{doing_irq::none};
	uint64_t tick_count{ 0ULL };
};

Registers& GRegisters();

namespace irq {
	//irq flags
	static constexpr uint32_t dpcm 				= 0x01U;
	static constexpr uint32_t apu_frame			= 0x02U;
	static constexpr uint32_t mapper1			= 0x04U;
	static constexpr uint32_t mapper2			= 0x08U;
	static constexpr uint32_t fds				= 0x10U;
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
void set_nz(uint32_t  val);

static constexpr uint32_t CARRY_FLAG_MASK			= 0x01;
static constexpr uint32_t ZERO_FLAG_MASK			= 0x02;
static constexpr uint32_t IRQ_DISABLE_FLAG_MASK	= 0x04;
static constexpr uint32_t DECIMAL_FLAG_MASK		= 0x08;
static constexpr uint32_t BFLAG_01_MASK			= 0x10;
static constexpr uint32_t BFLAG_10_MASK			= 0x20;
static constexpr uint32_t OVERFLOW_FLAG_MASK		= 0x40;
static constexpr uint32_t NEGATIVE_FLAG_MASK		= 0x80;

//Used in PLP and RTI because these bits are ignored
static constexpr uint32_t BFLAG_CLEAR_MASK			= 0xCF;