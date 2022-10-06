#include <gfx/input.h>

uint cpu_get_joy1()
{
	auto keyStates = getKeyStates();

	uint ret = 0;

	for(uint key = 0; auto keyState : keyStates)
	{
		ret |= (keyState & 1) << key++;
	}

	return ret;
}

uint cpu_get_joy2()
{
	return 0;
}

