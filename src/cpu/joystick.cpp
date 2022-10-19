
uint32_t cpu_get_joy1()
{
	static std::array<uint32_t, 8> keyStates;

	getKeyStates(keyStates);

	uint ret = 0;

	for(uint key = 0; auto keyState : keyStates)
	{
		ret |= (keyState & 1) << key++;
	}

	return ret;
}

uint32_t cpu_get_joy2()
{
	return 0;
}

