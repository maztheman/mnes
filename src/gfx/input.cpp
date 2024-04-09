#include "input.h"
#include "MainLayer.h"

namespace {
	auto pMainWindow = Main().get();
}

bool mnes::gfx::is_key_pressed(GfxEngine::Key key)
{
	return pMainWindow->IsKeyPressed(key);
}