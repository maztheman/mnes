#pragma once

#include <PixelType.h>

#include <cstdint>
#include <filesystem>
#include <span>

namespace GfxEngine::Platform::OpenGL330 {

namespace fs = std::filesystem;


class OpenGLTexture
{
public:
  OpenGLTexture(PixelType type, int width, int height);

  void SetData(std::span<uint8_t> data);

  uint32_t GetTextureID() const { return m_TextureID; }

private:
  uint32_t m_TextureID;
  PixelType m_Type;
  int m_Width;
  int m_Height;
};


}// namespace GfxEngine::Platform::OpenGL330