SET(SRCS 
    OpenGLImGui.cpp
    OpenGLPlatform.cpp
    OpenGLTexture.cpp
    OpenGLWindow.cpp
)

target_sources(GfxEngine PRIVATE ${SRCS})
