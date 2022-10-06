#pragma once

#include <common/ines_format.h>

#define MAPPER_NROM		0
#define MAPPER_MMC1		1
#define MAPPER_UNROM	2
#define MAPPER_CNROM	3
#define MAPPER_MMC3		4
#define MAPPER_MMC5		5
#define MAPPER_AOROM	7
#define MAPPER_MMC2		9
#define MAPPER_MMC4		10

#define NROM		0
#define MMC1		1
#define UNROM	2
#define CNROM	3
#define MMC3		4
#define MMC5		5
#define AOROM	7
#define MMC2		9
#define MMC4		10

struct mapper_t {
	uint (*read_memory)(uint address);
	uint (*read_ppu_memory)(uint address);
	void (*write_memory)(uint address, uint value);
	void (*do_cpu_cycle)();
	void (*do_ppu_cycle)();
	void (*reset)();
	uint mapper_no;
	bool m_bSaveRam;
};

inline uint ppu_read_nop(uint) { return 0; }

#define SETUP_MAPPER(mapper_no, readmem, writemem, docpu, doppu, reset, ppuread) mapper_t mapr##mapper_no = {readmem, ppuread, writemem, docpu, doppu, reset, mapper_no, false};

mapper_t* get_mapper(uint mapper_no);

void SetHorizontalMirror();
void SetVerticalMirror();
void SetOneScreenMirror();

extern uchar*		g_ROM[8];
extern vuchar		g_arRawData;
extern ines_format	g_ines_format;
