#ifndef __PPU_MEMORY_H__
#define __PPU_MEMORY_H__

typedef unsigned char uchar;
typedef unsigned int uint;


uint ppu_memory_main_read(uint address);
void ppu_memory_main_write(uint address, uint value);


extern uchar* g_PPUTable[8];
extern uchar* g_Tables[4];
extern uchar g_Palette[64];
extern uchar g_NTRam[0x800];

#endif