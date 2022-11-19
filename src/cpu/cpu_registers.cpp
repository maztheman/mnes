#include "cpu_registers.h"

Registers& GRegisters()
{
	static Registers instance;
	return instance;
}

void set_dpcm_irq()
{
	GRegisters().irq |= irq::dpcm;
}

void set_apu_frame_irq()
{
	GRegisters().irq |= irq::apu_frame;
}

void set_mapper1_irq() {
	GRegisters().irq |= irq::mapper1;
}

void set_mapper2_irq() {
	GRegisters().irq |= irq::mapper2;
}

void set_fds_irq() {
	GRegisters().irq |= irq::fds;
}

void clear_dpcm_irq()
{
	GRegisters().irq &= ~irq::dpcm;
}

void clear_apu_frame_irq()
{
	GRegisters().irq &= ~irq::apu_frame;
}

void clear_mapper1_irq() {
	GRegisters().irq &= ~irq::mapper1;
}

void clear_mapper2_irq() {
	GRegisters().irq &= ~irq::mapper2;
}

void clear_fds_irq() {
	GRegisters().irq &= ~irq::fds;
}

bool get_dpcm_irq() {
	return (GRegisters().irq & irq::dpcm) == irq::dpcm;
}

bool get_apu_frame_irq() {
	return (GRegisters().irq & irq::apu_frame) == irq::apu_frame;
}

bool get_mapper1_irq() {
	return (GRegisters().irq & irq::mapper1) == irq::mapper1;
}

bool get_mapper2_irq() {
	return (GRegisters().irq & irq::mapper2) == irq::mapper2;
}

bool get_fds_irq() {
	return (GRegisters().irq & irq::fds) == irq::fds;
}

bool is_carry() {
	return ((GRegisters().status & 1) == 1);
}

bool IF_OVERFLOW() {
	return ((GRegisters().status & 64) == 64);
}

bool IF_SIGN() {
	return ((GRegisters().status & 128) == 128);
}

bool IF_ZERO() {
	return ((GRegisters().status & 2) == 2);
}

bool IF_INTERRUPT() {
	return ((GRegisters().status & 4) == 4);
}

void set_carry(bool bValue) {
	if (bValue) {
		GRegisters().status |= 1;
	} else {
		GRegisters().status &= 0xFE;
	}
}

void SET_OVERFLOW(bool bValue) {
	if (bValue) {
		GRegisters().status |= 64;
	} else {
		GRegisters().status &= 0xBF;
	}
}

void SET_SIGN(bool bValue) {
	if (bValue) {
		GRegisters().status |= 128;
	} else {
		GRegisters().status &= 0x7F;
	}
}

void SET_ZERO(bool bValue) {
	if (bValue) {
		GRegisters().status |= 2;
	} else {
		GRegisters().status &= 0xFD;
	}
}

void SET_INTERRUPT(bool bValue) {
	if (bValue) {
		GRegisters().status |= 4;
	} else {
		GRegisters().status &= 0xFB;
	}
}

void SET_DECIMAL(bool bValue) {
	if (bValue) {
		GRegisters().status |= 8;
	} else {
		GRegisters().status &= 0xF7;
	}
}

void set_nz(uint val) {
	SET_SIGN((val & 0x80) == 0x80);
	SET_ZERO((val & 0xFF) == 0);
}