#pragma once

using uint = unsigned int;

void cpu_do_cycle();

void cpu_initialize(void* hInstance, void* hWnd);

uint cpu_get_joy1();
uint cpu_get_joy2();

#define NMILO   0xFFFA
#define NMIHI   0xFFFB
#define RESETLO	0xFFFC
#define RESETHI	0xFFFD
#define BRKLO	0xFFFE
#define BRKHI   0xFFFF
