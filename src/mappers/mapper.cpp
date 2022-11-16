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
	fmt::print(stderr, "using mapper no {}\n", mapper_no);

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
	auto& tables = Tables();
	auto ntram = NTRam();
	tables[0] = tables[1] = ntram.data();
	tables[2] = tables[3] = ntram.subspan(0x400).data();
}

void SetVerticalMirror()
{
	auto& tables = Tables();
	auto ntram = NTRam();

	tables[0] = tables[2] = ntram.data();
	tables[1] = tables[3] = ntram.subspan(0x400).data();
}

void SetOneScreenMirror()
{ 
	auto& tables = Tables();
	tables[1] = tables[3] = tables[0] = tables[2] = NTRam().data();
}