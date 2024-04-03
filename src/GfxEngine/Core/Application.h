#pragma once

#include "../PlatformType.h"
#include "LayerT.h"
#include <memory>
#include <vector>

#include <glm/glm.hpp>

namespace GfxEngine::Core {

template<PlatformType T> class Application
{
  using Window = typename T::Window;
  using WindowHandle = typename T::WindowHandle;

public:
  using RefLayer = std::shared_ptr<LayerT<T>>;
  Application(int width, int height, std::string_view title)
  {
    s_Application = this;
    m_MainWindowHandle = m_MainWindow.Create(width, height, title);
    m_MainWindow.SetOnWindowClose(Application::OnWindowClose);
    m_MainWindow.SetOnKeyEvent(Application::OnKeyEvent);
    T::SetClearColor({ 0.6, 0.4, 0.7, 1.0 });
  }

  virtual ~Application() {}

  void Run()
  {
    T::InitializeImGuiLayer(m_MainWindowHandle);

    auto now = std::chrono::high_resolution_clock::now();
    auto last = now;

    while (!m_ShouldExit) {
      T::PollEvents();

      last = now;
      now = std::chrono::high_resolution_clock::now();

      Timediff ts = std::chrono::duration_cast<Timediff>(now - last);

      for (auto &layer : m_Layers) { layer->OnUpdate(ts); }

      T::Clear();

      T::ImGuiBeginRender();

      for (auto &layer : m_Layers) { layer->OnImGui(); }

      T::ImGuiEndRender();

      T::SwapBuffers(m_MainWindowHandle);
    }
  }

  void AddLayer(const RefLayer &layer)
  {
    layer->OnAttach(m_MainWindowHandle);
    m_Layers.push_back(layer);
  }

  void SetClearColor(const glm::vec4 &color) { T::SetClearColor(color); }

  Window &GetWindow() { return m_MainWindow; }

private:
  inline static Application *s_Application = nullptr;


  static void OnWindowClose() { s_Application->m_ShouldExit = true; }

  static void OnKeyEvent(Key key, int action, int extra)
  {
    bool handled = false;
    for (auto &layer : s_Application->m_Layers) {
      if (handled) { break; }
      layer->OnKeyEvent(key, action, extra, handled);
    }
  }

  Window m_MainWindow;
  WindowHandle m_MainWindowHandle;
  bool m_ShouldExit{ false };
  std::vector<RefLayer> m_Layers;
};


}// namespace GfxEngine::Core