#include "minput.hpp"

minput::input_t joy1;

static uint s_Keys[8] = {
	MI_APOSTROPHE, MI_SEMICOLON,  //A, B
	MI_K, MI_L, //select, start
	MI_W, MI_S, MI_A, MI_D //up, down, left, right
};

static uint s_States[8] = {0};

uint cpu_get_joy1()
{
	memset(&s_States[0], 0, sizeof(uint) * 8);
	joy1.readKeys(s_Keys, s_States, 8);
	uint ret = 0;
	for(uint key = 0;key < 8;key++) {
		ret |= (s_States[key] & 1) << key;
	}
	return ret;
}

uint cpu_get_joy2()
{
	return 0;
}

