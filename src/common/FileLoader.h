#pragma once

#include <filesystem>

namespace mnes::file
{
  bool load_nes_file(const std::filesystem::path &fileName);
}