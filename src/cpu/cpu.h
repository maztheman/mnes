#pragma once

#if 0
#include <unordered_map>

#include <KeyType.h>

using KEY = GfxEngine::Key;

static std::unordered_map<KEY, std::pair<const int, bool>> KEYS = {
    std::make_pair(KEY::Apos, std::make_pair(0, false)),
    std::make_pair(KEY::Semicolon, std::make_pair(1, false)),
    std::make_pair(KEY::K, std::make_pair(2, false)),
    std::make_pair(KEY::L, std::make_pair(3, false)),
    std::make_pair(KEY::W, std::make_pair(4, false)),
    std::make_pair(KEY::S, std::make_pair(5, false)),
    std::make_pair(KEY::A, std::make_pair(6, false)),
    std::make_pair(KEY::D, std::make_pair(7, false))
};
#endif

void cpu_do_cycle();
void cpu_cycle_reset(int64_t count);
void cpu_cycle_set(int64_t count);
int64_t cpu_get_cycle();

void cpu_reset();
void cpu_initialize();

uint cpu_get_joy1();
uint cpu_get_joy2();

constexpr auto NMILO = 0xFFFA;
constexpr auto NMIHI = 0xFFFB;
constexpr auto RESETLO = 0xFFFC;
constexpr auto RESETHI = 0xFFFD;
constexpr auto BRKLO = 0xFFFE;
constexpr auto BRKHI = 0xFFFF;
