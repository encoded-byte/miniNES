#pragma once

#include "devices/Bus.h"
#include "machine/Device.h"


class APU : public Device
{
private:
	static const uint16_t frame_length{29830};
	static const uint8_t  length_counter_table[32];
	static const uint8_t  pulse_duty_table[4];
	static const uint16_t dmc_period_table[16];
	static const uint16_t noise_period_table[16];
	int16_t pulse_dac[32];
	int16_t tnd_dac[16][16][128];

	struct Envelope
	{
		bool    reload;
		bool    const_volume;
		bool    loop;
		uint8_t counter;
		uint8_t volume : 4;
		uint8_t decay : 4;
	};

	struct Sweep
	{
		bool    reload;
		bool    enable;
		bool    subtract;
		bool    carry;
		uint8_t counter;
		uint8_t shift;
		uint8_t period : 3;
	};

	struct Pulse
	{
		void reset();
		void clock();
		void quarter_frame();
		void half_frame();

		Envelope envelope;
		Sweep    sweep;
		uint32_t length_counter;
		uint8_t  duty : 2;
		uint8_t  duty_counter : 3;
		uint16_t period : 11;
		uint32_t period_counter;
		uint8_t  output;
	};

	struct Triangle
	{
		void reset();
		void clock();
		void quarter_frame();
		void half_frame();

		bool     length_counter_halt;
		bool     linear_reload;
		uint32_t length_counter;
		uint8_t  linear_length;
		uint8_t  linear_counter;
		uint16_t period : 11;
		uint32_t period_counter;
		uint8_t  step_counter : 5;
		uint8_t  output;
	};

	struct Noise
	{
		void reset();
		void clock();
		void quarter_frame();
		void half_frame();

		Envelope envelope;
		bool     mode;
		uint32_t length_counter;
		uint8_t  period : 4;
		uint32_t period_counter;
		uint16_t shift : 15;
		uint8_t  output;
	};

	struct DMC
	{
		void reset();
		void clock();

		Bus      &bus;
		bool     loop;
		bool     buffer_valid;
		bool     sample_valid;
		uint32_t length_counter;
		uint16_t length_load;
		uint16_t addr;
		uint16_t addr_load;
		uint8_t  period : 4;
		uint32_t period_counter;
		uint8_t  cycle_counter;
		uint8_t  bit_counter : 3;
		uint8_t  buffer;
		uint8_t  sample;
		uint8_t  output;
	};

	// Channels
	Pulse    pulse[2];
	Triangle triangle;
	Noise    noise;
	DMC      dmc;

	// Registers
	bool     extra_step;
	uint8_t  channels_enabled;

	// Status
	uint8_t  frame_step : 2;
	int32_t  frame_divider;
	uint16_t frame_index;
	int32_t  sample_divider;
	uint16_t sample_index;
	bool     irq_enable;
	bool     irq_request;

	// IO
	int16_t  output[800] {0x00};

	// Signals
	void quarter_frame();

public:
	APU(Bus &bus);

	// Device interface
	uint8_t read(uint16_t addr) override;
	void write(uint16_t addr, uint8_t data) override;

	// Status
	bool is_irq() const { return irq_request; }

	// IO
	const int16_t* get_output() const { return output; }

	// Signals
	void reset();
	void clock();
};
