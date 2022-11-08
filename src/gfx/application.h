#pragma once

#include <array>
#include <functional>
#include <mutex>

#include <Core/Application.h>
#include <Core/LayerT.h>

//add some sort of rom state struct to keep some of the globals locally here.
class MnesApplication : public GfxEngine::Core::Application<ApplicationPlatform>
{
public:
    MnesApplication();

};
