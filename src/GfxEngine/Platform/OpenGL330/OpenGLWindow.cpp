#include "OpenGLWindow.h"

#include "OpenGLPlatform.h"

#include <GLFW/glfw3.h>

namespace GfxEngine::Platform::OpenGL330::Window
{

WindowHandle OpenGLWindow::Create(int width, int height, std::string_view title)
{
    m_Data.width = width;
    m_Data.height = height;
    m_Handle = OpenGL330Platform::Create(width, height, &m_Data, title);
    return m_Handle;
}

void OpenGLWindow::SetOnWindowClose(OnWindowCloseFn onClose)
{
    //because the window events are already hooked up all we must do is set the member here and the plumbing will take care of rest
    m_Data.onWindowCloseFn = onClose;
}

void OpenGLWindow::SetOnKeyEvent(OnKeyEvent onKeyEvent)
{
    m_Data.onKeyEvent = onKeyEvent;
}

}