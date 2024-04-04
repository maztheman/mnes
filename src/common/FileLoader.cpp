#include "FileLoader.h"

#include "File.h"
#include "ines_format.h"

#include <mappers/mapper.h>

#include <fstream>

// mappers
vuchar g_arRawData;

// maybe attach the mapper to the application instead of global variable ?

CFileLoader::CFileLoader() {}

CFileLoader::~CFileLoader() {}

bool CFileLoader::LoadRom(const std::filesystem::path &fileName)
{
  std::ifstream file(fileName, std::ios::binary | std::ios::in);

  if (!file) {
    fmt::print(stderr, "Could not open rom {}\n", fileName.generic_string());
    return false;
  }

  auto &format = nes_format();

  file.read(reinterpret_cast<char *>(&format), 16);

  if (!(format.reserved[0] == 'N' && format.reserved[1] == 'E' && format.reserved[2] == 'S'
        && format.file_version == 0x1A)) {
    return false;// wrong format i guess
  }

  uint nMapperNo = ((format.rom_control_1 & 0xF0) >> 4) | (format.rom_control_2 & 0xF0);

  set_mapper(nMapperNo);
  auto mp = current_mapper();

  if (mp == nullptr) {
    fmt::print(stderr, "mapper {} is not supported\n", nMapperNo);
    return false;
  }

  // bool ines20 = (format.rom_control_1 & 0xC) == 0x8;

  // prg + chr + trainer
  std::streamsize nFileSize =
    (format.prg_rom_count * 16384) + (format.chr_rom_count * 8192) + (((format.rom_control_1 & 4) == 4) ? 512 : 0);

  fmt::print(stderr, "Detected rom size of {}\n", nFileSize);

  set_romdata_from_stream(file, nFileSize);

  mp->reset();

  return true;
}