#pragma once

namespace mnes::cpu {

void do_cycle();
void cycle_reset(int64_t count);
void cycle_set(int64_t count);
int64_t get_cycle();

void reset();
void initialize();

uint get_joy1();
uint get_joy2();

constexpr auto NMILO = 0xFFFA;
constexpr auto NMIHI = 0xFFFB;
constexpr auto RESETLO = 0xFFFC;
constexpr auto RESETHI = 0xFFFD;
constexpr auto BRKLO = 0xFFFE;
constexpr auto BRKHI = 0xFFFF;

}// namespace mnes::cpu