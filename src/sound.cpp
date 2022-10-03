static constexpr uint s_Noise[16] = {
  4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068  
};

static constexpr uint s_DMC[16] = {
    428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106,  84,  72,  54
};

static constexpr uint s_LengthCounter[0x20] = {
	10,254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14,
	12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30
};

static constexpr uint s_Duty[4] = {
	0x40, 0x60, 0x78, 0x9F
};

static constexpr uint s_Sequencer[32] = {
	15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0,
	0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15
};

static constexpr double s_DMCFreq[16] = {
    4181.71,
    4709.93,
    5264.04,
    5593.04,
    6257.95,
    7046.35,
    7919.35,
    8363.42,
    9419.86,
    11186.1,
    12604.0,
    13982.6,
    16884.6,
    21306.8,
    24858.0,
    33143.9
};

struct sweep_unit_t
{
	sweep_unit_t(bool is_pulse_1, uint& target_timer)
		: pulse_1_fg(is_pulse_1)
		, timer(target_timer)
	{

	}

	bool pulse_1_fg{ false };
	bool enabled{ false };
	uint period{ 1 };
	bool negative{ false };
	uint shift_count;
	uint& timer;
	uint divider_count;
	bool reload_fg{ false };

	void set(uint value) {
		enabled = (value & 0x80) == 0x80;
		period = ((value & 0x70) >> 4) + 1;
		negative = (value & 8) == 8;
		shift_count = value & 7;
		reload_fg = true;
	}

	void update_timer()
	{
		if (negative) {
			if (pulse_1_fg) {
				timer += -(int)(timer >> shift_count) - 1;
			} else {
				timer += -(int)(timer >> shift_count);
			}
		} else {
			timer += (timer >> shift_count);
		}
	}

	bool is_muting() const {
		return timer > 0x7FF || timer < 8;
	}

	void clock()
	{
		if (divider_count == 0 && enabled && is_muting() == false) {
			update_timer();
		}
		if (divider_count == 0 || reload_fg) {
			divider_count = period;
			reload_fg = false;
		} else {
			divider_count--;
		}
	}
};

struct right_shift_reg8_t
{
	uint value{ 0 };

	uint clock() {
		uint retval = value & 1;

	}
};

struct envelope_t {
	
	envelope_t(bool& lo, bool& dis)
	: loop(lo)
	, disable(dis)
	, must_reload(false)
	, reload(0)
	, period(0)
	, counter(0)
	{

	}

	void clock() {
		if (must_reload) {
			period = reload + 1;
			counter = 15;
			must_reload = false;
			return;
		}

		period--;
		if (period == 0) {
			output_clock();
		}
	}

	void output_clock() {
			if (loop && counter == 0) {
				counter = 15;
			} else if (counter != 0) {
				counter--;
			}
			period = reload + 1;
	}

	bool&		loop;
	bool&		disable;
	bool		must_reload;

	uint reload;
	uint period;
	uint counter{ 0 };

};

struct square_channel_t
{
	//54.6hz to 12400hz

	square_channel_t(bool is_pulse_1 = false)
	: enabled(false)
	, envelope(loop_envelope, envelope_disable)
	, sweep{ is_pulse_1, timer }
	, length(0U)
	{

	}

	void reset() {
		enabled = false;
		length = 0U;
	}

	void set0(uint value) {
		duty = (value & 0xC0) >> 6;
		loop_envelope = (value & 0x20) == 0x20;
		envelope_disable = (value & 0x10) == 0x10;
		envelope.reload = (value & 0xF);
	}

	void set1(uint value) {
		sweep.set(value);
	}

	void set2(uint value) {
		timer &= 0x700;
		timer |= (value & 0xFF);
	}

	void set3(uint value) {
		length_idx = (value & 0xF8) >> 3;
		timer &= 0xFF;
		timer |= (value & 0x7) << 8;
		if (enabled) {
			length = s_LengthCounter[length_idx];
		}
		envelope.must_reload = true;
	}

	void clock_timer()
	{
		timer--;
		if (timer == 0) {
			
		}
	}

	uint volume() const 
	{

		//sweep unit says no no
		if (timer > 0x7FF) {
			return 0;
		}

		//sweet unit says no no
		if (timer < 8) {
			return 0;
		}

		return envelope.reload;
	}

	void clock_length() {
		if (loop_envelope == false && length != 0) {
			length--;
		}
	}

	//if initial state is any different bad things will happen ?

	bool			enabled{ false };

	//Register 3
	uint			timer{ 0 };

	//Register 1
	uint			duty{ 0 };
	bool			loop_envelope{ false };
	bool			envelope_disable{ false };
	envelope_t		envelope;
	//Register 2
	sweep_unit_t	sweep;
	//Register 4
	uint			length_idx{ 0 };
	uint			length{ 0 };
};

struct triangle_channel_t
{
	//27.3hz to 55900hz

	triangle_channel_t()
	: enabled(false)
	, length_idx(0U)
	, length(0U)
	{

	}

	void reset() {
		enabled = false;
		length_idx = 0U;
		length = 0U;
	}

	void set0(uint value) {
		length_counter_halt = (value & 0x80) == 0x80;
		linear_counter_reload = (value & 0x7F);
	}

	void set1(uint value) {
		timer &= 0x700;
		timer |= (value & 0xFF);
	}

	void set2(uint value) {
		length_idx = (value & 0xF8) >> 3;
		timer &= 0xFF;
		timer |= (value & 0x7) << 8;
		linear_counter_must_reload = true;
		if (enabled) {
			length = s_LengthCounter[length_idx];
		}
	}

	uint volume() const {
		return linear_counter_reload & 0xF;
	}

	void clock_length() {
		if (length_counter_halt == false && length != 0) {
			length--;
		}
	}

	void clock_linear() {
		if (linear_counter_must_reload) {
			linear_counter = linear_counter_reload;
			linear_counter_must_reload = false;
		} else {
			if (linear_counter != 0) {
				linear_counter--;
			}
		}
	}

	void clock_timer()
	{
		timer--;
	}

	bool			enabled;
	//Register 1
	bool			length_counter_halt;
	uint			linear_counter_reload;
	uint			linear_counter;
	//Register 2
	uint			timer{ 0 };
	//Register 3
	uint			length_idx;
	uint			length;

	//Register 3 Side-effect
	bool			linear_counter_must_reload;
};

struct noise_channel_t
{
	//29.3hz to 447000hz

	noise_channel_t()
	: enabled(false)
	, envelope(loop_envelope, envelope_disable)
	, length_idx(0U)
	, length(0U)
	{

	}
	
	void reset() {
		enabled = false;
		length = length_idx = 0U;
	}
	
	void set0(uint value) {
		loop_envelope = (value & 0x20) == 0x20;
		envelope_disable = (value & 0x10) == 0x10;
		envelope.reload = (value & 0xF);
	}

	void set1(uint value) {
		short_mode = (value & 0x80) == 0x80;
		period_idx = (value & 0xF);
		timer = s_Noise[period_idx];
	}

	void set2(uint value) {
		length_idx = (value >> 3) & 0x1F;
		if (enabled) {
			length = s_LengthCounter[length_idx];
		}
		envelope.must_reload = true;
	}

	uint volume() const {
		return envelope.reload;
	}

	void clock_length() {
		if (loop_envelope == false && length != 0) {
			length--;
		}
	}

	void clock_timer()
	{
		timer--;
	}
	

	bool			enabled;
	//Register 1
	bool			loop_envelope;
	bool			envelope_disable;
	envelope_t		envelope;
	//Register 2
	bool			short_mode;
	uint			period_idx;
	uint			period{ 0 };
	uint			timer;
	//Register 3
	uint			length_idx;
	uint			length;
};

struct memory_reader_t
{
	uint address;
	uint remaining;
	uint sample_byte;
};

struct dmc_output_unit_t
{

};

struct dmc_channel_t
{
	dmc_channel_t()
	: enabled(false)
	{

	}
	
	void set0(uint value) {
		irq_enable = (value & 0x80) == 0x80;
		loop = (value & 0x40) == 0x40;
		frequency_idx = (value & 0xF);
		timer = s_DMC[frequency_idx];
	}

	void set1(uint value) {
		DAC = (value & 0x7F);
	}

	void set2(uint value) {
		memory_rdr.address = sample_address = value;
	}

	void set3(uint value) {
		memory_rdr.remaining = sample_length = value;
	}

	uint actual_address() const {
		return 0xC000 | ((sample_address & 0xFF) << 6);
	}

	uint actual_length() const {
		return ((sample_length & 0xFF) << 4) | 1;
	}

	uint volume() const {
		return 0;
	}

	void clock_timer()
	{
		timer -= 2;
		if (timer == 0) {
			output_clock();
			timer = s_DMC[frequency_idx];
		}
	}

	void output_clock()
	{

	}
	
	bool			enabled;

	//Register 1
	bool			irq_enable;
	bool			loop;
	uint			frequency_idx{ 0 };
	uint			frequency;
	uint			timer;
	//Register 2
	uint			DAC;
	//Register 3
	uint			sample_address;
	uint			current_address;
	//Register 4
	uint			sample_length;
	memory_reader_t	memory_rdr;
};

struct soundregs_t
{
	soundregs_t()
		: should_reset(false)
	{

	}

	square_channel_t		square_1{ true };
	square_channel_t		square_2;
	triangle_channel_t		triangle;
	noise_channel_t			noise;
	dmc_channel_t			dmc;
	//Register 4017
	bool					five_frame_cycle;
	bool					disable_frame_interrupt{ false };
	//frame sequencer
	uint			seq_counter;
	bool			should_reset;
	uint			should_reset_at;

	void set4015(uint value) {
		//mute channels
		square_1.enabled = (value & 1) == 1;
		if ((value & 1) == 0) {
			square_1.length = 0;
		}
		square_2.enabled = (value & 2) == 2;
		if ((value & 2) == 0) {
			square_2.length = 0;
		}
		triangle.enabled = (value & 4) == 4;
		if ((value & 4) == 0) {
			triangle.length = 0;
		}
		noise.enabled = (value & 8) == 8;
		if ((value & 8) == 0) {
			noise.length = 0;
		}
	}

	void set4017(uint value) {
		five_frame_cycle = (value & 0x80) == 0x80;
		disable_frame_interrupt = (value & 0x40) == 0x40;

		if (disable_frame_interrupt) {
			clear_apu_frame_irq();
		}

		should_reset = true;
		should_reset_at = s_HalfCycles + (2 + (s_HalfCycles & 1));
	}

	uint get4015() {
		uint retval = 0;
		retval |= (square_1.length > 0) ? 0x01 : 0x00;
		retval |= (square_2.length > 0) ? 0x02 : 0x00;
		retval |= (triangle.length > 0) ? 0x04 : 0x00;
		retval |= (noise.length > 0)	? 0x08 : 0x00;
		retval |= (false)				? 0x10 : 0x00;//dmc remaining bytes
		retval |= (get_apu_frame_irq())	? 0x40 : 0x00;
		retval |= (get_dpcm_irq())		? 0x80 : 0x00;//dmc irq
		
		clear_apu_frame_irq();
		
		return retval;
	}

	void set_frame_irq() {
		if (!disable_frame_interrupt) {
			set_apu_frame_irq();
		}
	}

	void clock_length() {
		square_1.clock_length();
		square_2.clock_length();
		triangle.clock_length();
		noise.clock_length();
	}

	void clock_sweep() {
		square_1.sweep.clock();
		square_2.sweep.clock();
	}

	void clock_envelope() {
		square_1.envelope.clock();
		square_2.envelope.clock();
		noise.envelope.clock();
		triangle.clock_linear();
	}

	void clock_timer() {
		static bool bEven = false;

		if (bEven) {
			square_1.clock_timer();
			square_2.clock_timer();
		}

		triangle.clock_timer();

		bEven = !bEven;
	}
};

static soundregs_t sregs;

//15 bit
struct noise_shift_reg_t
{
    uint reg;  
    void set(uint value) {
        reg = value & 0x7FFF;
    }
    
    void shift() {
        uint a = (reg & 1) ? 1 : 0;
        uint b = 0;
        if (sregs.noise.short_mode) {
            b = (reg & 0x40) ? 1 : 0;
        } else {
            b = (reg & 0x02) ? 1 : 0;
        }
        uint c = a ^ b;
        reg >>= 1;
        reg |= c ? 0x4000 : 0x0000;
    }
    
};

static noise_shift_reg_t noise_regs;
