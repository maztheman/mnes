#include "apu.h"

#include <cpu/cpu_registers.h>

//#include "msound.h"

#include <array>

void apu_mixer();

//static device_list_t		device_list = nullptr;
//static device_t				audio_device = nullptr;
//static device_format_t		s_audio_format;

static uint s_HalfCycles;
static int s_ticks = 0;
static int s_prev_sound_tick = 0;
static int s_prev_ticks = 0;
static uint s_AudioLength;

using pulse_snd_t = std::array<double, 31>;

constexpr std::array<double, 31> pulse_snd_init()
{
	std::array<double, 31> retval = { 0.0 };
	retval[0] = 0.0;
	for (size_t i = 1; i < 31; i++) {
		retval[i] = 95.52 / (8128.0 / static_cast<double>(i) + 100.0);
	}
	return retval;
}

constexpr std::array<double, 203> tnd_snd_init()
{
	std::array<double, 203> retval = { 0.0 };
	retval[0] = 0.0;
	for (size_t i = 1; i < 203; i++) {
		retval[i] = 163.67 / ((24329.0 / static_cast<double>(i)) + 100.0);
	}
	return retval;
}

static constexpr std::array<double, 31> pulse_snd{pulse_snd_init()};
static constexpr std::array<double, 203> tnd_snd{ tnd_snd_init()};

#include "sound.cpp"

static constexpr int ciSoundHz = 44100;
static constexpr int ciNesHz = 1789773;

static uint32_t s_samples[0x6000];

/*
							95.88
pulse_out =		------------------------------------
				  (8128 / (pulse1 + pulse2)) + 100


										     159.79
tnd_out =		-------------------------------------------------------------
											1
				 ----------------------------------------------------- + 100
				  (triangle / 8227) + (noise / 12241) + (dmc / 22638)


*/



void apu_initialize()
{
	s_HalfCycles = 0;
	s_prev_sound_tick = 0;

#if 0
	device_list = msound_device_list();
	if (device_list) {
		uint indx = msound_device_list_get_count(device_list);
		if (indx > 0) {
			audio_device = msound_device_list_get_item(device_list, 0);
			s_audio_format.hertz = ciSoundHz;
			s_audio_format.bits_per_sample = 16;
			s_audio_format.channels = 2;
			msound_device_initialize(audio_device, &s_audio_format);
			s_AudioLength = msound_device_get_buffer_size(audio_device);
		}
	}
#endif

	printf("apu initialize finished\n");
}

void apu_destroy()
{
#if 0
	if (device_list) {
		msound_device_list_destroy(device_list);
	}
#endif
}

void apu_reset()
{
	s_HalfCycles = 0;
	s_prev_sound_tick = 0;
}

void apu_do_cycle()
{
	bool bClockLength = false;
	bool bClockEnvelope = false;
	bool bTrySetInterrupt = false;
	bool bResetCycle = false;

	if (sregs.should_reset && sregs.should_reset_at == s_HalfCycles) {
		bResetCycle = true;
		if (sregs.five_frame_cycle) {
			bClockLength = true;
			bClockEnvelope = true;
		}
		sregs.should_reset = false;
	}

	if (s_HalfCycles == 7457) {			//Step 1
		bClockEnvelope = true;
	} else if (s_HalfCycles == 14913) {	//Step 2
		bClockEnvelope = true;
		bClockLength = true;
	} else if (s_HalfCycles == 22371) {	//Step 3
		bClockEnvelope = true;
	} else if (s_HalfCycles == 29828) {
		if (sregs.five_frame_cycle == false) {
			bTrySetInterrupt = true;
		}
	} else if (s_HalfCycles == 29829) {	//Step 4
		if (sregs.five_frame_cycle == false) {
			bTrySetInterrupt = true;
			bClockLength = bClockEnvelope = true;
		}
	} else if (s_HalfCycles == 29830) {
		if (sregs.five_frame_cycle == false) {
			bTrySetInterrupt = true;
			bResetCycle = true;
		}
	} else if (s_HalfCycles == 37281) {	//Step 5
		bClockEnvelope = bClockLength = true;
	} else if (s_HalfCycles == 37282) {
		bResetCycle = true;
	}

	s_HalfCycles++;

	if (bTrySetInterrupt) {
		sregs.set_frame_irq();
	}

	if (bClockEnvelope) {
		sregs.clock_envelope();
	}

	if (bClockLength) {
		sregs.clock_length();
		sregs.clock_sweep();
	}

	sregs.clock_timer();

	if (bResetCycle) {
		s_HalfCycles = 0;
	}

	//No idea what this does execept that it says samples per second ( x cpu cycles
	//then if it is a multiple of nes hz, mix the channels for sound

	int sound_tick = ciSoundHz * s_ticks++ / ciNesHz;
	if (sound_tick != s_prev_sound_tick) {
		s_prev_ticks = s_ticks - 1;
		apu_mixer();
		s_prev_sound_tick = sound_tick;
	} else {
		//build up a buffer of 40 samples ? 40 or 41, whatever s_ticks is
		uint idx = static_cast<uint32_t>(s_ticks - s_prev_ticks);

		uint pulse1 = sregs.square_1.volume(),
			pulse2 = 3,
			triangle = 0,
			noise = 0,
			dmc = 0;

		auto pulse_out = pulse_snd[pulse1 + pulse2];
		auto tnd_out = tnd_snd[3 * triangle + 2 * noise + dmc];

		auto output = pulse_out + tnd_out;
		uint16_t digital = static_cast<uint16_t>(65535 * output);

		s_samples[idx] = digital | digital << 16U;
	}

}

void apu_memory_write(uint addr, uint value) {
	addr &= 0x1F;
	switch (addr) {
		//Square 1
	case 0x00:
		sregs.square_1.set0(value);
		break;
	case 0x01:
		sregs.square_1.set1(value);
		break;
	case 0x02:
		sregs.square_1.set2(value);
		break;
	case 0x03:
		sregs.square_1.set3(value);
		break;
		//Square 2
	case 0x04:
		sregs.square_2.set0(value);
		break;
	case 0x05:
		sregs.square_2.set1(value);
		break;
	case 0x06:
		sregs.square_2.set2(value);
		break;
	case 0x07:
		sregs.square_2.set3(value);
		break;
		//Triangle
	case 0x08:
		sregs.triangle.set0(value);
		break;
	case 0x0A:
		sregs.triangle.set1(value);
		break;
	case 0x0B:
		sregs.triangle.set2(value);
		break;
		//Noise
	case 0x0C:
		sregs.noise.set0(value);
		break;
	case 0x0E:
		sregs.noise.set1(value);
		break;
	case 0x0F:
		sregs.noise.set2(value);
		break;
		//DMC
	case 0x10:
		sregs.dmc.set0(value);
		break;
	case 0x11:
		sregs.dmc.set1(value);
		break;
	case 0x12:
		sregs.dmc.set2(value);
		break;
	case 0x13:
		sregs.dmc.set3(value);
		break;
		//Control
	case 0x15:
		sregs.set4015(value);
		break;
		//Frame Counter
	case 0x17:
		sregs.set4017(value);
		break;
	}
}

uint apu_memory_read(uint address)
{
	uint value = address >> 8;//open bus
	address &= 0x1F;
	if (address == 0x15) {
		value = (sregs.get4015() & 0xDF);
	}
	return value;
}

void apu_mixer()
{
	

}