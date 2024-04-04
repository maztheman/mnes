#pragma once

#include <concepts>
#include <string_view>

#include "KeyType.h"

namespace GfxEngine {
using namespace std::string_view_literals;

namespace internals {
  template<typename T>
  concept has_typenames = requires
  {
    typename T::WindowHandle;
    typename T::Window;
    typename T::WindowData;
    typename T::TextureType;
  };

  template<typename T>
  concept has_static_functions = requires(typename T::WindowHandle Handle, typename T::WindowData windowData)
  {
    T::Init();
    {
      T::Create(0, 0, &windowData, ""sv)
      } -> std::same_as<typename T::WindowHandle>;
    T::InitializeImGuiLayer(Handle);
    T::PollEvents();
    T::ImGuiBeginRender();
    T::ImGuiEndRender();
    T::Clear();
    {
      T::IsKeyPressed(Handle, Key::A)
      } -> std::same_as<bool>;
  };
}// namespace internals

template<typename T>
concept PlatformType = internals::has_typenames<T> && internals::has_static_functions<T>;


}// namespace GfxEngine