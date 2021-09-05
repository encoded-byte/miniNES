#include "devices/DMA.h"


//////////////////////////////////////////////////////////////////////////////
//
//                             DEVICE INTERFACE
//
//////////////////////////////////////////////////////////////////////////////


// Device: Read
uint8_t DMA::read(uint16_t addr)
{
	return 0;
}

// Device: Write
void DMA::write(uint16_t addr, uint8_t data)
{
	page = data;
	offset = 0;
	cycle = 0;
	ready = 0;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void DMA::reset()
{
	page = 0;
	offset = 0;
	cycle = 0;
	ready = 1;
}

// Signal: Clock
void DMA::clock()
{
	if (!ready && (cycle++ & 1))
	{
		uint8_t data = bus.read((page << 8) | offset);
		ppu.write_oam(offset, data);

		if (++offset == 0)
			ready = 1;
	}
}
