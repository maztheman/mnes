#pragma once

#include <common/ines_format.h>
#include <ios>

namespace mnes::mappers {
constexpr auto NROM = 0;
constexpr auto MMC1 = 1;
constexpr auto UNROM = 2;
constexpr auto CNROM = 3;
constexpr auto MMC3 = 4;
constexpr auto MMC5 = 5;
constexpr auto AOROM = 7;
constexpr auto MMC2 = 9;
constexpr auto MMC4 = 10;
constexpr auto CART_VRAM_SIZE = 0x2000U;

using cart_vram_t = fixed_span_bytes<CART_VRAM_SIZE>;

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

inline uint ppu_read_nop(uint) { return 0; }

void set_mapper(uint mapper_no);
mapper_t *current();

void SetHorizontalMirror();
void SetVerticalMirror();
void SetOneScreenMirror();

void set_romdata_from_stream(std::ifstream &stream, std::streamsize size);
std::span<uint8_t> current_raw_data();
std::array<uint8_t *, 8> &current_rom_data();
mnes::file::ines_format_t &current_nes_format();
cart_vram_t cart_vram();

}// namespace mnes::mappers
