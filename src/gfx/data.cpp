#include "data.h"
#include <memory>

static std::unique_ptr<uint8_t[]> screenData = std::make_unique<uint8_t[]>(SCREEN_DATA_SIZE);

std::span<uint8_t, SCREEN_DATA_SIZE> getScreenData()
{
  return std::span<uint8_t, SCREEN_DATA_SIZE>(screenData.get(), SCREEN_DATA_SIZE);
}