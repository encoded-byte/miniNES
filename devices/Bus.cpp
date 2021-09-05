#include "devices/Bus.h"


void Bus::add(Device &device, uint16_t begin, uint16_t end, bool read_only)
{
	map.push_back({ device, begin, end, read_only });
}


//////////////////////////////////////////////////////////////////////////////
//
//                             DEVICE INTERFACE
//
//////////////////////////////////////////////////////////////////////////////


// Device: Read
uint8_t Bus::read(uint16_t addr)
{
	for (const auto &region : map)
	{
		if (addr >= region.begin && addr <= region.end)
			return region.device.read(addr);
	}

	return 0;
}

// Device: Write
void Bus::write(uint16_t addr, uint8_t data)
{
	for (const auto &region : map)
	{
		if (addr >= region.begin && addr <= region.end && !region.read_only)
		{
			region.device.write(addr, data);
			break;
		}
	}
}
