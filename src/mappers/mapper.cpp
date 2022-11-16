#include "mapper.h"

#include "aorom.h"
#include "cnrom.h"
#include "nrom.h"
#include "unrom.h"
#include "mmc1.h"
#include "mmc2.h"
#include "mmc3.h"
#include "mmc4.h"
#include "mmc5.h"

#include "memory.h"

#include <ppu/ppu_memory.h>

#include <cstring>

static inline mapper_t*& GMapper()
{
	static mapper_t* instance;
	return instance;
}

mapper_t* get_mapper(uint mapper_no)
{
	switch(mapper_no) 
	{
		using namespace mnes::mappers;
		case MMC2:
			return &mapperMMC2();
		case MMC3:
			return &mapperMMC3();
		case MMC4:
			return &mapperMMC4();
		case MMC5:
			return &mapperMMC5();
		case MMC1:
			return &mapperMMC1();
		case CNROM:
			return &mapperCNROM();
		case UNROM:
			return &mapperUNROM();
		case NROM:
			return &mapperNROM();
		case AOROM:
			return &mapperAOROM();
	}
	return nullptr;
}

void set_mapper(uint mapper_no)
{
	GMapper() = get_mapper(mapper_no);
}

mapper_t* current_mapper()
{
	return GMapper();
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