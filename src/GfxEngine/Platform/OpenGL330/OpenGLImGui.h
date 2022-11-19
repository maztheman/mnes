#pragma once

#include "OpenGLWindow.h"

namespace GfxEngine::Platform::OpenGL330::OpenGLImGui
{

    void Initialize(Window::WindowHandle window);
    void BeginRender();
    void EndRender();
    void Shutdown();

}