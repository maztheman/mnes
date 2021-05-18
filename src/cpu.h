#ifndef __CPU_H__
#define __CPU_H__

typedef unsigned int uint;

void cpu_execute();
void cpu_do_cycle();
void cpu_execute_nmi();
void cpu_execute_irq();

void cpu_initialize(void* hInstance, void* hWnd);

uint cpu_get_joy1();
uint cpu_get_joy2();

#define NMILO   0xFFFA
#define NMIHI   0xFFFB
#define RESETLO	0xFFFC
#define RESETHI	0xFFFD
#define BRKLO	0xFFFE
#define BRKHI   0xFFFF

#endif