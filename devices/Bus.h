#pragma once

#include <vector>
#include "machine/Device.h"


class Bus : public Device
{
private:
	struct Region
	{
		Device &device;
		const uint16_t begin;
		const uint16_t end;
		const bool read_only;
	};
	std::vector<Region> map;

public:
	Bus() {}
	void add(Device &device, uint16_t begin, uint16_t end, bool read_only);

	// Device interface
	uint8_t read(uint16_t addr) override;
	void write(uint16_t addr, uint8_t data) override;
};
