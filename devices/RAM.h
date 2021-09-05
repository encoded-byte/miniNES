#pragma once

#include <vector>
#include "machine/Device.h"


class RAM : public Device
{
private:
	std::vector<uint8_t> mem;

public:
	RAM() : mem(2048, 0) {}

	// Device interface
	uint8_t read(uint16_t addr) override;
	void write(uint16_t addr, uint8_t data) override;
};
