#pragma once
#include <memory>

namespace mnes::log {
    void initialize();
    std::shared_ptr<spdlog::logger>& main();
    std::shared_ptr<spdlog::logger>& ppu();

#define MLOG(x, ...) SPDLOG_LOGGER_TRACE(mnes::log::main(), x, ##__VA_ARGS__)
#define MLOG_PPU(x, ...) SPDLOG_LOGGER_TRACE(mnes::log::ppu(), x, ##__VA_ARGS__)
}