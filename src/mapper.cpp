#include "mapper.h"
#include "ppu_memory.h"
#include "memory.h"
#include <cstring>

//idea stolen from nesemu2, hah!
#define CASE_MAPPER(m)\
case m: {\
	extern mapper_t mapr##m;\
	return &mapr##m;\
}\

mapper_t* get_mapper(uint mapper_no)
{
	switch(mapper_no) {
		CASE_MAPPER(NROM)
		CASE_MAPPER(UNROM)
		CASE_MAPPER(AOROM)
		CASE_MAPPER(CNROM)
		CASE_MAPPER(MMC1)
		CASE_MAPPER(MMC2)
		CASE_MAPPER(MMC3)
		CASE_MAPPER(MMC4)
		CASE_MAPPER(MMC5)
	}
	return nullptr;
}

uchar* g_ROM[8] = {0};

void SetHorizontalMirror()
{
	g_Tables[0] = g_Tables[1] = &g_NTRam[0x000];
	g_Tables[2] = g_Tables[3] = &g_NTRam[0x400];
}

void SetVerticalMirror()
{ 
	g_Tables[0] = g_Tables[2] = &g_NTRam[0x000];
	g_Tables[1] = g_Tables[3] = &g_NTRam[0x400];
}

void SetOneScreenMirror()
{ 
	g_Tables[1] = g_Tables[3] = g_Tables[0] = g_Tables[2] = &g_NTRam[0x000];
}