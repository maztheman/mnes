#pragma once

#include "../PlatformType.h"
#include "../Timediff.h"
#include "../KeyType.h"

namespace GfxEngine::Core
{

//kind of redundant since its already templated
template<typename T, typename PT>
concept LayerType = requires(T a, typename PT::WindowHandle Handle, Timediff ts)
{
    a.OnAttach(Handle);
    a.OnUpdate(ts);
    a.OnDetach();
    a.OnImGui();
};

template<PlatformType T>
class LayerT
{
public:
    using WindowHandle = typename T::WindowHandle;
    using Timediff = GfxEngine::Timediff;
    using TextureType = typename T::TextureType;
    using Key = GfxEngine::Key;

    virtual ~LayerT() {}
    virtual void OnAttach(WindowHandle window) = 0;
    virtual void OnUpdate(Timediff ts) = 0;
    virtual void OnDetach() = 0;
    virtual void OnImGui() = 0;
    virtual void OnKeyEvent(Key key, int action, int extra, bool& handled) = 0;
};



}