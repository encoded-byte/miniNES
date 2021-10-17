#include "devices/APU.h"


const uint8_t APU::length_counter_table[32]
{
	0x0a, 0xfe, 0x14, 0x02, 0x28, 0x04, 0x50, 0x06, 0xa0, 0x08, 0x3c, 0x0a, 0x0e, 0x0c, 0x1a, 0x0e,
	0x0c, 0x10, 0x18, 0x12, 0x30, 0x14, 0x60, 0x16, 0xc0, 0x18, 0x48, 0x1a, 0x10, 0x1c, 0x20, 0x1e,
};

const uint8_t APU::pulse_duty_table[4]
{
	0x01, 0x03, 0x0f, 0xfc,
};

const uint16_t APU::noise_period_table[16]
{
	0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0060, 0x0080, 0x00a0,
	0x00ca, 0x00fe, 0x017c, 0x01fc, 0x02fa, 0x03f8, 0x07f2, 0x0fe4,
};

const uint16_t APU::dmc_period_table[16]
{
	0x01ac, 0x017c, 0x0154, 0x0140, 0x011e, 0x00fe, 0x00e2, 0x00d6,
	0x00be, 0x00a0, 0x008e, 0x0080, 0x006a, 0x0054, 0x0048, 0x0036,
};

APU::APU(Bus &bus) : dmc{bus}
{
	pulse[0].sweep.carry = 1;
	pulse[1].sweep.carry = 0;

	for (uint8_t amp = 0; amp < 32; amp++)
	{
		if (amp == 0)
			pulse_dac[amp] = 0;
		else
			pulse_dac[amp] = 16384.0 * (95.88 / (100.0 + (8128.0 / amp)));
	}

	for (uint8_t triangle_amp = 0; triangle_amp < 16; triangle_amp++)
	{
		for (uint8_t noise_amp = 0; noise_amp < 16; noise_amp++)
		{
			for (uint8_t dmc_amp = 0; dmc_amp < 128; dmc_amp++)
			{
				if (triangle_amp == 0 && noise_amp == 0 && dmc_amp == 0)
					tnd_dac[triangle_amp][noise_amp][dmc_amp] = 0;
				else
					tnd_dac[triangle_amp][noise_amp][dmc_amp] =
						16384.0 * (159.79 / (100.0 + (1.0 / ((triangle_amp / 8227.0) + (noise_amp / 12241.0) + (dmc_amp / 22638.0)))));
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                             DEVICE INTERFACE
//
//////////////////////////////////////////////////////////////////////////////


// Device: Read
uint8_t APU::read(uint16_t addr)
{
	uint8_t data = 0;

	if (addr == 0x4015)
	{
		data |= pulse[0].length_counter ? 0x01 : 0;
		data |= pulse[1].length_counter ? 0x02 : 0;
		data |= triangle.length_counter ? 0x04 : 0;
		data |= noise.length_counter ? 0x08 : 0;
		data |= dmc.length_counter ? 0x10 : 0;
		data |= irq_request ? 0x40 : 0;
		irq_request = 0;
	}

	return data;
}

// Device: Write
void APU::write(uint16_t addr, uint8_t data)
{
	uint8_t i = (addr >> 2) & 1;

	switch (addr)
	{
	case 0x4000:
	case 0x4004:
		pulse[i].envelope.volume = data & 0x0f;
		pulse[i].envelope.const_volume = data & 0x10;
		pulse[i].envelope.loop = data & 0x20;
		pulse[i].duty = data >> 6;
		break;

	case 0x4001:
	case 0x4005:
		pulse[i].sweep.shift = data & 0x07;
		pulse[i].sweep.subtract = data & 0x08;
		pulse[i].sweep.period = (data & 0x70) >> 4;
		pulse[i].sweep.enable = data & 0x80;
		pulse[i].sweep.reload = 1;
		break;

	case 0x4002:
	case 0x4006:
		pulse[i].period = (pulse[i].period & 0x0700) | data;
		break;

	case 0x4003:
	case 0x4007:
		pulse[i].period = (pulse[i].period & 0x00ff) | (data << 8);
		pulse[i].duty_counter = 0;
		pulse[i].envelope.reload = 1;
		if (channels_enabled & (1 << i))
			pulse[i].length_counter = length_counter_table[(data >> 3) & 0x1f];
		break;

	case 0x4008:
		triangle.linear_length = data & 0x7f;
		triangle.length_counter_halt = data & 0x80;
		break;

	case 0x400a:
		triangle.period = (triangle.period & 0x0700) | data;
		break;

	case 0x400b:
		triangle.period = (triangle.period & 0x00ff) | (data << 8);
		triangle.linear_reload = 1;
		if (channels_enabled & 0x04)
			triangle.length_counter = length_counter_table[(data >> 3) & 0x1f];
		break;

	case 0x400c:
		noise.envelope.volume = data & 0x0f;
		noise.envelope.const_volume = data & 0x10;
		noise.envelope.loop = data & 0x20;
		break;

	case 0x400e:
		noise.period = data & 0x0f;
		noise.mode = data & 0x80;
		break;

	case 0x400f:
		noise.envelope.reload = 1;
		if (channels_enabled & 0x08)
			noise.length_counter = length_counter_table[(data >> 3) & 0x1f];
		break;

	case 0x4010:
		dmc.period = data & 0x0f;
		dmc.loop = data & 0x40;
		break;

	case 0x4011:
		dmc.output = data & 0x7f;
		break;

	case 0x4012:
		dmc.addr_load = 0xc000 | (data << 6);
		break;

	case 0x4013:
		dmc.length_load = (data << 4) + 1;
		break;

	case 0x4015:
		channels_enabled = data & 0x1f;

		if ((data & 0x01) == 0)
			pulse[0].length_counter = 0;
		if ((data & 0x02) == 0)
			pulse[1].length_counter = 0;
		if ((data & 0x04) == 0)
			triangle.length_counter = 0;
		if ((data & 0x08) == 0)
			noise.length_counter = 0;

		if ((data & 0x10) == 0)
		{
			dmc.length_counter = 0;
			dmc.cycle_counter = 0;
		}
		else if (dmc.length_counter == 0)
		{
			dmc.addr = dmc.addr_load;
			dmc.length_counter = dmc.length_load;
		}
		break;

	case 0x4017:
		irq_enable = ~data & 0x40;
		extra_step = data & 0x80;
		frame_step = 0;
		if (extra_step)
			quarter_frame();
		if (!irq_enable)
			irq_request = 0;
		frame_divider = frame_length;
		break;
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void APU::reset()
{
	pulse[0].reset();
	pulse[1].reset();
	triangle.reset();
	noise.reset();
	dmc.reset();

	extra_step = 0;
	channels_enabled = 0;
	frame_step = 0;
	frame_divider = 1;
	frame_index = 0;
	sample_divider = 1;
	sample_index = 0;
	irq_enable = 0;
	irq_request = 0;
}

// Signal: Clock
void APU::clock()
{
	pulse[0].clock();
	pulse[1].clock();
	triangle.clock();
	noise.clock();
	dmc.clock();

	frame_divider -= 4;
	if (frame_divider <= 0)
	{
		quarter_frame();
		frame_divider += frame_length;
	}

	sample_divider -= 800;
	if (sample_divider <= 0)
	{
		output[sample_index] = pulse_dac[pulse[0].output + pulse[1].output];
		output[sample_index] += tnd_dac[triangle.output][noise.output][dmc.output];
		sample_index++;
		sample_divider += frame_length;
	}

	if (++frame_index == frame_length)
	{
		frame_index = 0;
		sample_index = 0;
	}
}

// Signal: Quarter Frame
void APU::quarter_frame()
{
	if ((frame_step & 0x01) == 0)
	{
		pulse[0].half_frame();
		pulse[1].half_frame();
		triangle.half_frame();
		noise.half_frame();
	}

	pulse[0].quarter_frame();
	pulse[1].quarter_frame();
	triangle.quarter_frame();
	noise.quarter_frame();

	if (++frame_step == 0)
	{
		if (extra_step)
			frame_divider += frame_length;
		else if (irq_enable)
			irq_request = 1;
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                              PULSE CHANNEL
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset (Pulse)
void APU::Pulse::reset()
{
	envelope.reload = 0;
	envelope.const_volume = 0;
	envelope.loop = 0;
	envelope.counter = 0;
	envelope.volume = 0;
	envelope.decay = 0;

	sweep.reload = 0;
	sweep.enable = 0;
	sweep.subtract = 0;
	sweep.counter = 1;
	sweep.shift = 0;
	sweep.period = 0;

	length_counter = 0;
	duty = 0;
	duty_counter = 0;
	period = 0;
	period_counter = 1;
}

// Signal: Clock (Pulse)
void APU::Pulse::clock()
{
	output = 0;
	if (period > 0x07ff)
		return;
	if (sweep.subtract == 0 && ((period + (period >> sweep.shift)) & 0x0800))
		return;
	if (length_counter == 0)
		return;

	bool cycle = (pulse_duty_table[duty] << duty_counter) & 0x80;
	if (cycle && period >= 8)
		output = envelope.const_volume ? envelope.volume : envelope.decay;

	if (--period_counter == 0)
	{
		period_counter = (period + 1) << 1;
		duty_counter--;
	}
}

// Signal: Quarter Frame (Pulse)
void APU::Pulse::quarter_frame()
{
	if (--envelope.counter == 0)
	{
		envelope.counter = envelope.volume + 1;
		if (envelope.decay || envelope.loop)
			envelope.decay--;
	}

	if (envelope.reload)
	{
		envelope.reload = 0;
		envelope.counter = envelope.volume + 1;
		envelope.decay = 0x0f;
	}
}

// Signal: Half Frame (Pulse)
void APU::Pulse::half_frame()
{
	if (!envelope.loop && length_counter)
		length_counter--;

	if (--sweep.counter == 0)
	{
		sweep.counter = sweep.period + 1;
		if (sweep.enable && sweep.shift && period > 8)
		{
			uint16_t increment = period >> sweep.shift;
			if (sweep.subtract)
				period -= increment - sweep.carry;
			else if ((period + increment) < 0x0800)
				period += increment;
		}
	}

	if (sweep.reload)
	{
		sweep.reload = 0;
		sweep.counter = sweep.period + 1;
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                             TRIANGLE CHANNEL
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset (Triangle)
void APU::Triangle::reset()
{
	length_counter_halt = 0;
	linear_reload = 0;
	length_counter = 0;
	linear_length = 0;
	linear_counter = 0;
	period = 0;
	period_counter = 1;
	step_counter = 0;
}

// Signal: Clock (Triangle)
void APU::Triangle::clock()
{
	output = step_counter & 0x0f;
	if ((step_counter & 0x10) == 0)
		output ^= 0x0f;

	if (length_counter == 0 || linear_counter == 0)
		return;

	if (--period_counter == 0)
	{
		step_counter++;
		period_counter = period + 1;
	}
}

// Signal: Quarter Frame (Triangle)
void APU::Triangle::quarter_frame()
{
	if (linear_reload)
		linear_counter = linear_length;
	else if (linear_counter)
		linear_counter--;

	if (!length_counter_halt)
		linear_reload = 0;
}

// Signal: Half Frame (Triangle)
void APU::Triangle::half_frame()
{
	if (!length_counter_halt && length_counter)
		length_counter--;
}


//////////////////////////////////////////////////////////////////////////////
//
//                              NOISE CHANNEL
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset (Noise)
void APU::Noise::reset()
{
	envelope.reload = 0;
	envelope.const_volume = 0;
	envelope.loop = 0;
	envelope.counter = 0;
	envelope.volume = 0;
	envelope.decay = 0;

	mode = 0;
	length_counter = 0;
	period = 0;
	period_counter = 1;
	shift = 1;
}

// Signal: Clock (Noise)
void APU::Noise::clock()
{
	output = 0;
	if (length_counter == 0)
		return;

	if (shift & 0x01)
		output = envelope.const_volume ? envelope.volume : envelope.decay;

	if (--period_counter == 0)
	{
		uint16_t feedback = (shift & 0x01) ^ ((shift >> (mode ? 6 : 1)) & 0x01);
		shift = (shift >> 1) | (feedback << 14);
		period_counter = noise_period_table[period];
	}
}

// Signal: Quarter Frame (Noise)
void APU::Noise::quarter_frame()
{
	if (--envelope.counter == 0)
	{
		envelope.counter = envelope.volume + 1;
		if (envelope.decay || envelope.loop)
			envelope.decay--;
	}

	if (envelope.reload)
	{
		envelope.reload = 0;
		envelope.counter = envelope.volume + 1;
		envelope.decay = 0x0f;
	}
}

// Signal: Half Frame (Noise)
void APU::Noise::half_frame()
{
	if (!envelope.loop && length_counter)
		length_counter--;
}


//////////////////////////////////////////////////////////////////////////////
//
//                               DMC CHANNEL
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset (DMC)
void APU::DMC::reset()
{
	loop = 0;
	buffer_valid = 0;
	sample_valid = 0;
	length_counter = 0;
	length_load = 0;
	addr = 0;
	addr_load = 0;
	period = 0;
	period_counter = dmc_period_table[0];
	cycle_counter = 0;
	bit_counter = 0;
	buffer = 0;
	sample = 0;
	output = 0;
}

// Signal: Clock (DMC)
void APU::DMC::clock()
{
	if (cycle_counter && --cycle_counter == 0)
	{
		buffer = bus.read(addr++);
		buffer_valid = 1;

		if (--length_counter == 0 && loop)
		{
			addr = addr_load;
			length_counter = length_load;
		}
	}

	if (--period_counter == 0)
	{
		if (sample_valid)
		{
			int8_t increment = ((sample >> bit_counter) & 0x01) ? 2 : -2;
			uint8_t output_load = output + increment;
			if (output_load <= 0x7f)
				output = output_load;
		}

		if (++bit_counter == 0)
		{
			sample_valid = 0;
			if (buffer_valid)
			{
				sample = buffer;
				sample_valid = 1;
				buffer_valid = 0;
			}
		}

		period_counter = dmc_period_table[period];
	}

	if (length_counter && cycle_counter == 0 && !buffer_valid)
		cycle_counter = 4;
}
