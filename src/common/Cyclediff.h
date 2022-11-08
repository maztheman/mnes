#pragma once

#include <chrono>

using CPUCycleRatio = std::ratio<1, 1789773>;
using Cyclediff = std::chrono::duration<int64_t, CPUCycleRatio>;
