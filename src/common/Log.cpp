#include "Log.h"

namespace {
  std::shared_ptr<spdlog::logger> mainlogger;
  std::shared_ptr<spdlog::logger> ppulogger;
}

void mnes::log::initialize()
{
  mainlogger = spdlog::get("main");
  ppulogger = spdlog::get("ppu");
}

std::shared_ptr<spdlog::logger>& mnes::log::main()
{
  return mainlogger;
}

std::shared_ptr<spdlog::logger>& mnes::log::ppu()
{
  return ppulogger;
}

