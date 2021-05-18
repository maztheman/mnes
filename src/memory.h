#ifndef __MEMORY_H__
#define __MEMORY_H__

typedef unsigned int uint;
typedef unsigned char uchar;

uint ext_memory_read(uint address);
void ext_memory_write(uint address, uint value);

uint memory_main_read(uint address);
void memory_main_write(uint address, uint value);

void memory_pc_process();

extern uchar g_SPR_RAM[0x100];
extern uchar g_TMP_SPR_RAM[32];
extern uchar g_SRAM[0x2000];


#endif