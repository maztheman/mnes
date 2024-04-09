#include <gfx/MainLayer.h>
#include <gfx/application.h>
#include <memory>
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <common/Log.h>

int main()
{
  auto ppulogger = spdlog::basic_logger_mt<spdlog::async_factory>("ppu", "logs/ppu.txt", true);
  auto cpulogger = spdlog::basic_logger_mt<spdlog::async_factory>("main", "logs/main.txt", true);
  mnes::log::initialize();
  spdlog::flush_every(std::chrono::seconds(3));
  // create the application
  MnesApplication app;
  app.AddLayer(Main());
  // run it
  app.Run();

  return 0;
}
