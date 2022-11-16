#pragma once

void cpu_do_cycle();
void cpu_cycle_reset(int64_t count);
void cpu_cycle_set(int64_t count);
int64_t cpu_get_cycle();

void cpu_reset();
void cpu_initialize();

uint cpu_get_joy1();
uint cpu_get_joy2();

constexpr auto NMILO = 0xFFFA;
constexpr auto NMIHI = 0xFFFB;
constexpr auto RESETLO = 0xFFFC;
constexpr auto RESETHI = 0xFFFD;
constexpr auto BRKLO = 0xFFFE;
constexpr auto BRKHI = 0xFFFF;
