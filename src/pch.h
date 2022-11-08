#include <vector>
#include <cstdint>
#include <string>

using uint = unsigned int;
using uchar = unsigned char;
using vuchar = std::vector<uchar>;

#include <mappers/mapper.h>

#include <fmt/format.h>

#include <PlatformType.h>
#include <Platform/OpenGL330/OpenGLPlatform.h>

using ApplicationPlatform = GfxEngine::Platform::OpenGL330::OpenGL330Platform;

