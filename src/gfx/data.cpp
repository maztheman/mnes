#include "data.h"
#include <memory>
#include "MainLayer.h"
#include "common/control_flags.h"

#define NS mnes::gfx::

using namespace mnes;
using namespace mnes::gfx;

namespace {
std::unique_ptr<uint8_t[]> screenData = std::make_unique<uint8_t[]>(SCREEN_DATA_SIZE);
constexpr auto END_VISIBLE_SCREEN_OFFSET = 256U * 240U * 3U;
constexpr auto OFF_SCREEN_SIZE = 256U * 16U * 3U;
}

std::span<uint8_t, SCREEN_DATA_SIZE> NS screen_data()
{
  return std::span<uint8_t, SCREEN_DATA_SIZE>(screenData.get(), SCREEN_DATA_SIZE);
}

void NS update_texture_from_screen_data()
{
  g_bDisplayReady = g_bPatternTableReady = g_bNameTableReady = true;
  Main()->UpdateTexture();
}

void NS clear_off_screen_data()
{
  auto tx = screen_data().subspan(END_VISIBLE_SCREEN_OFFSET, OFF_SCREEN_SIZE);
  memset(tx.data(), 0, tx.size_bytes());
}