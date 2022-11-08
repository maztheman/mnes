#pragma once

#include "OpenGLWindowData.h"

#include <string_view>

struct GLFWwindow;

namespace GfxEngine::Platform::OpenGL330
{

using namespace ::std::string_view_literals;

namespace Window 
{

using WindowHandle = GLFWwindow*;

class OpenGLWindow
{
public:

    WindowHandle Create(int width, int height, std::string_view title = ""sv);

    void SetOnWindowClose(OnWindowCloseFn onClose);
    void SetOnKeyEvent(OnKeyEvent onKeyEvent);

private:

    OpenGLWindowData m_Data;
    WindowHandle m_Handle;
};


}

}