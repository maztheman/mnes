#include "OpenGLTexture.h"

#include <glad/glad.h>

namespace GfxEngine::Platform::OpenGL330
{

static GLenum PixelTypeToOpenGLFormat(PixelType type)
{
    switch(type)
    {
        using enum PixelType;
        case RGB8:   
            return GL_RGB;
        case RGBA8:
            return GL_RGBA;
    }
}

static GLenum PixelTypeToOpenGLInternalFormat(PixelType type)
{
    switch(type)
    {
        using enum PixelType;
        case RGB8:   
            return GL_RGB8;
        case RGBA8:
            return GL_RGBA8;
    }
}

static GLenum PixelTypeToOpenGLType(PixelType)
{
    return GL_UNSIGNED_BYTE;
}

OpenGLTexture::OpenGLTexture(PixelType type, int width, int height)
: m_Type(type)
, m_Width(width)
, m_Height(height)
{
    glGenTextures(1, &m_TextureID );
    glBindTexture(GL_TEXTURE_2D, m_TextureID );
    glTexImage2D(GL_TEXTURE_2D, 0, PixelTypeToOpenGLInternalFormat(type), width, height, 0, PixelTypeToOpenGLFormat(type), PixelTypeToOpenGLType(type), nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Linear Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Linear Filtering
}

void OpenGLTexture::SetData(std::span<uint8_t> data)
{
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, PixelTypeToOpenGLFormat(m_Type), PixelTypeToOpenGLType(m_Type), data.data());
}


}