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

#include <cstring>
#include <fstream>

namespace {
mapper_t *get_mapper(uint mapper_no);
}

struct nes_data
{
  mapper_t *mapper{ nullptr };
  ines_format_t ines_format;
  std::unique_ptr<uint8_t[]> romData;
  size_t romDataSize;
  std::array<uint8_t *, 8> rom = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
};

namespace {
inline nes_data &GNesData()
{
  static nes_data instance;
  return instance;
}
}// namespace

ines_format_t &nes_format() { return GNesData().ines_format; }

std::span<uint8_t> RawData() { return { GNesData().romData.get(), GNesData().romDataSize }; }

std::array<uint8_t *, 8> &RomData() { return GNesData().rom; }

void set_mapper(uint mapper_no) { GNesData().mapper = get_mapper(mapper_no); }

mapper_t *current_mapper() { return GNesData().mapper; }

namespace {

mapper_t *get_mapper(uint mapper_no)
{
  fmt::print(stderr, "using mapper no {}\n", mapper_no);

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

void set_romdata_from_stream(std::ifstream &stream, std::streamsize size)
{
  auto &rom = GNesData().romData;

  auto sz = static_cast<size_t>(size);

  rom = std::make_unique<uint8_t[]>(sz);
  GNesData().romDataSize = sz;

  stream.read(reinterpret_cast<char *>(rom.get()), size);
}

void SetHorizontalMirror()
{
  auto &tables = Tables();
  auto ntram = NTRam();
  tables[0] = tables[1] = ntram.data();
  tables[2] = tables[3] = ntram.subspan(0x400).data();
}

void SetVerticalMirror()
{
  auto &tables = Tables();
  auto ntram = NTRam();

  tables[0] = tables[2] = ntram.data();
  tables[1] = tables[3] = ntram.subspan(0x400).data();
}

void SetOneScreenMirror()
{
  auto &tables = Tables();
  tables[1] = tables[3] = tables[0] = tables[2] = NTRam().data();
}
