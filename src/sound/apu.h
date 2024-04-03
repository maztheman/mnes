#pragma once


namespace mnes::apu {

void initialize();
void destroy();
void reset();
void do_cycle();

void write(uint32_t addr, uint32_t value);
uint read(uint32_t address);

}
