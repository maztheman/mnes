
using KEY = GfxEngine::Key;

namespace {
constexpr std::array<KEY, 8> MNES_KEYS = { KEY::Apos, KEY::Semicolon, KEY::K, KEY::L, KEY::W, KEY::S, KEY::A, KEY::D };
}

uint32_t mnes::cpu::get_joy1()
{
  uint32_t ret = 0;

  for (uint32_t key = 0; auto &myKey : MNES_KEYS) {
    bool pressed = gfx::is_key_pressed(myKey);
    ret |= (pressed ? 1U : 0U) << key++;
  }

  return ret;
}

uint32_t mnes::cpu::get_joy2() { return 0; }
