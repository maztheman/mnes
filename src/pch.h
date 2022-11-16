#include <vector>
#include <cstdint>
#include <string>
#include <span>

using uint = unsigned int;
using vuchar = std::vector<uint8_t>;
using span_bytes = std::span<uint8_t>;

template<size_t SZ>
using fixed_span_bytes = std::span<uint8_t, SZ>;

#include <mappers/mapper.h>

#include <fmt/format.h>

#include <PlatformType.h>
#include <Platform/OpenGL330/OpenGLPlatform.h>

using ApplicationPlatform = GfxEngine::Platform::OpenGL330::OpenGL330Platform;

