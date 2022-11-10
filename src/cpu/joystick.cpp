
using KEY = GfxEngine::Key;

static constexpr std::array<KEY, 8> MNES_KEYS = {
	KEY::Apos,
	KEY::Semicolon,
	KEY::K,
	KEY::L,
	KEY::W,
	KEY::S,
	KEY::A,
	KEY::D
};

uint32_t cpu_get_joy1()
{
	static MainLayer* TheMain = Main().get();

	uint ret = 0;

	for(uint key = 0; auto& myKey : MNES_KEYS)
	{
		bool pressed = TheMain->IsKeyPressed(myKey);
		ret |= (pressed ? 1U : 0U) << key++;
	}

	return ret;
}

uint32_t cpu_get_joy2()
{
	return 0;
}

