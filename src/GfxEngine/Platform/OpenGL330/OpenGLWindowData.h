#pragma once

#include "KeyType.h"

namespace GfxEngine::Platform::OpenGL330::Window
{

using OnWindowCloseFn = void(*)();
using OnKeyEvent = void(*)(Key key, int action, int extra);

struct OpenGLWindowData
{
    int width;
    int height;
    OnWindowCloseFn onWindowCloseFn = nullptr;
    OnKeyEvent onKeyEvent = nullptr;
};

}