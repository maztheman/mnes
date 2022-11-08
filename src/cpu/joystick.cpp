
uint32_t cpu_get_joy1()
{
	uint ret = 0;

	for(auto& myKey : KEYS)
	{
		ret |= (myKey.second.second ? 1 : 0) << myKey.second.first;
	}

	return ret;
}

uint32_t cpu_get_joy2()
{
	return 0;
}

