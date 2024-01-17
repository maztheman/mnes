#pragma once

#include "KeyType.h"

namespace GfxEngine::Platform::OpenGL330::Window
{

using OnWindowCloseFn = void(*)();
using OnKeyEvent = void(*)(Key key, int action, int extra);

struct OpenGLWindowData
{
    int width = 0;
    int height = 0;
    OnWindowCloseFn onWindowCloseFn = nullptr;
    OnKeyEvent onKeyEvent = nullptr;
};

}
