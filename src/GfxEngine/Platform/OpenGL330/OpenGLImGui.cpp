#include "OpenGLImGui.h"

#include "OpenGLPlatform.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <imgui_impl_glfw.cpp>
#include <imgui_impl_opengl3.cpp>

#include <imgui.h>

namespace GfxEngine::Platform::OpenGL330::OpenGLImGui {

void Initialize(Window::WindowHandle window)
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;

  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(OpenGL330Platform::IMGUI_EXTRA_INFO.data());
}

void BeginRender()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void EndRender()
{
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Shutdown()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

}// namespace GfxEngine::Platform::OpenGL330::OpenGLImGui
