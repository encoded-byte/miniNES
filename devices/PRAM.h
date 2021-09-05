#pragma once

#include <vector>
#include "machine/Device.h"


class PRAM : public Device
{
private:
	std::vector<uint8_t> mem;

public:
	PRAM() : mem(32, 0) {}

	// Device interface
	uint8_t read(uint16_t addr) override;
	void write(uint16_t addr, uint8_t data) override;
};
