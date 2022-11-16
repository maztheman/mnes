#pragma once

#include <common/ines_format.h>

namespace mnes::mappers
{
	constexpr auto NROM = 0;
	constexpr auto MMC1 = 1;
	constexpr auto UNROM = 2;
	constexpr auto CNROM = 3;
	constexpr auto MMC3 = 4;
	constexpr auto MMC5 = 5;
	constexpr auto AOROM = 7;
	constexpr auto MMC2 = 9;
	constexpr auto MMC4 = 10;
}

struct mapper_t 
{
	uint (*read_memory)(uint address);
	uint (*read_ppu_memory)(uint address);
	void (*write_memory)(uint address, uint value);
	void (*do_cpu_cycle)();
	void (*do_ppu_cycle)();
	void (*reset)();
	uint mapper_no;
	bool m_bSaveRam;
};

inline uint ppu_read_nop(uint)
{
	return 0;
}

mapper_t* get_mapper(uint mapper_no);
void set_mapper(uint mapper_no);
mapper_t* current_mapper();

void SetHorizontalMirror();
void SetVerticalMirror();
void SetOneScreenMirror();

void set_romdata_from_stream(std::ifstream& stream, std::streamsize size);
std::span<uint8_t> RawData();
std::array<uint8_t*, 8>& RomData();

ines_format_t& nes_format();
