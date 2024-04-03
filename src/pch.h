#include <array>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

using uint = unsigned int;
using vuchar = std::vector<uint8_t>;
using span_bytes = std::span<uint8_t>;

template<size_t SZ> using fixed_span_bytes = std::span<uint8_t, SZ>;

#include <mappers/mapper.h>

#include <fmt/format.h>

#include <Platform/OpenGL330/OpenGLPlatform.h>
#include <PlatformType.h>

using ApplicationPlatform = GfxEngine::Platform::OpenGL330::OpenGL330Platform;

template<typename T> static inline uint8_t TO_BYTE(T value) { return static_cast<uint8_t>(value & 255U); }