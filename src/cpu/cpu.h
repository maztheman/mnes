#pragma once

using uint = unsigned int;

void cpu_do_cycle();

void cpu_initialize(void* hInstance, void* hWnd);

uint cpu_get_joy1();
uint cpu_get_joy2();

constexpr auto NMILO = 0xFFFA;
constexpr auto NMIHI = 0xFFFB;
constexpr auto RESETLO = 0xFFFC;
constexpr auto RESETHI = 0xFFFD;
constexpr auto BRKLO = 0xFFFE;
constexpr auto BRKHI = 0xFFFF;
