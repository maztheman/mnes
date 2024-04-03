#pragma once

namespace mnes::cpu {

void reset_registers();
uint32_t program_counter();
uint64_t mnes_tick_count();

void set_dpcm_irq();
void set_apu_frame_irq();
void set_mapper1_irq();
void set_mapper2_irq();
void set_fds_irq();
void set_nmi();
void copy_nmi_to_prev();

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
void set_nz(uint32_t val);


}