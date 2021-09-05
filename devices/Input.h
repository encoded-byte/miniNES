#pragma once

#include "machine/Device.h"


class Input : public Device
{
private:
	uint8_t input[2];
	uint8_t buffer[2];
	bool strobe;

public:
	Input() {}

	// Device interface
	uint8_t read(uint16_t addr) override;
	void write(uint16_t addr, uint8_t data) override;

	// IO
	void set_input(uint8_t index, uint8_t data) { input[index] = data; }

	// Signals
	void reset();
};
