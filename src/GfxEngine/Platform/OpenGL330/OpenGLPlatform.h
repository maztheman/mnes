#pragma once

#include "OpenGLTexture.h"
#include "OpenGLWindow.h"
#include "OpenGLWindowData.h"

#include <glm/glm.hpp>

namespace GfxEngine::Platform::OpenGL330 {
/**
 * satisfies the PlatformType concept
 * has typename::WindowHandle
 * has typename::Window
 * has fn Init();
 */
class OpenGL330Platform
{
public:
  /**
   * Is native platform handle
   */
  using WindowHandle = GfxEngine::Platform::OpenGL330::Window::WindowHandle;
  using WindowData = GfxEngine::Platform::OpenGL330::Window::OpenGLWindowData;
  using TextureType = GfxEngine::Platform::OpenGL330::OpenGLTexture;

  /**
   * has Create(int, int, sv)
   */
  using Window = GfxEngine::Platform::OpenGL330::Window::OpenGLWindow;
  static inline constexpr auto IMGUI_EXTRA_INFO = "#version 330"sv;

  static void Init();
  static void Shutdown();
  static void PollEvents();
  static WindowHandle Create(int width, int height, WindowData *pUserData, std::string_view title = ""sv);

  static void InitializeImGuiLayer(WindowHandle window);
  static void ImGuiBeginRender();
  static void ImGuiEndRender();

  static void SetClearColor(const glm::vec4 &color);
  static void Clear();

  static void SwapBuffers(WindowHandle window);

  static bool IsKeyPressed(WindowHandle window, Key key);
};

}// namespace GfxEngine::Platform::OpenGL330