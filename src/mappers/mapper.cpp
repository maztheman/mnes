#include "mapper.h"

#include "aorom.h"
#include "cnrom.h"
#include "mmc1.h"
#include "mmc2.h"
#include "mmc3.h"
#include "mmc4.h"
#include "mmc5.h"
#include "nrom.h"
#include "unrom.h"

#include "memory.h"

#include <ppu/ppu_memory.h>
#include <common/Log.h>

#include <cstring>
#include <fstream>

#define NS mnes::mappers::

using namespace mnes;
using namespace mnes::mappers;

namespace {

mapper_t *get_mapper(uint mapper_no);


auto &ntable = ppu::memory::name_table();
auto ntRam = ppu::memory::NTRam();

struct nes_data
{
  mapper_t *mapper{ nullptr };
  mnes::file::ines_format_t ines_format;
  std::unique_ptr<uint8_t[]> romData;
  size_t romDataSize;
// TODO: Could this be a std::array<fixed_size_bytes<0x1000U>, 8> ?
  std::array<uint8_t *, 8> rom = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
  std::unique_ptr<uint8_t[]> vram = std::make_unique<uint8_t[]>(CART_VRAM_SIZE);
  
  void read_from_stream(std::ifstream &stream, std::streamsize size)
  {
    romDataSize = static_cast<size_t>(size);
    romData = std::make_unique<uint8_t[]>(romDataSize);
    stream.read(reinterpret_cast<char *>(romData.get()), size);
  }

  cart_vram_t get_cart_vram() { return cart_vram_t(vram.get(), CART_VRAM_SIZE); }
};

nes_data& singleton()
{
  static nes_data instance;
  return instance;
}
}// namespace

mnes::file::ines_format_t & NS current_nes_format() { return singleton().ines_format; }

std::span<uint8_t> NS current_raw_data() { return { singleton().romData.get(), singleton().romDataSize }; }

std::array<uint8_t *, 8> &NS current_rom_data() { return singleton().rom; }

void NS set_mapper(uint mapper_no) { singleton().mapper = get_mapper(mapper_no); }

NS mapper_t * NS current() { return singleton().mapper; }

namespace {

mapper_t *get_mapper(uint mapper_no)
{
  log::main()->info("using mapper no {}", mapper_no);

  switch (mapper_no) {
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
  default:
    return &mapperUNROM();
  }
  return nullptr;
}

}// namespace

void NS set_romdata_from_stream(std::ifstream &stream, std::streamsize size)
{
  singleton().read_from_stream(stream, size);
}

void NS SetHorizontalMirror()
{
  ntable[0] = ntable[1] = ntRam.data();
  ntable[2] = ntable[3] = ntRam.subspan(0x400).data();
}

void NS SetVerticalMirror()
{
  ntable[0] = ntable[2] = ntRam.data();
  ntable[1] = ntable[3] = ntRam.subspan(0x400).data();
}

void NS SetOneScreenMirror()
{
  ntable[1] = ntable[3] = ntable[0] = ntable[2] = ntRam.data();
}

cart_vram_t NS cart_vram()
{
  return singleton().get_cart_vram();
}