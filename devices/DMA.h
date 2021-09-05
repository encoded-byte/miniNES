#pragma once

#include "devices/Bus.h"
#include "devices/PPU.h"
#include "machine/Device.h"


class DMA : public Device
{
private:
	Bus &bus;
	PPU &ppu;
	uint8_t page;
	uint8_t offset;
	uint16_t cycle;
	bool ready;

public:
	DMA(Bus &bus, PPU &ppu) : bus(bus), ppu(ppu) {}

	// Device interface
	uint8_t read(uint16_t addr) override;
	void write(uint16_t addr, uint8_t data) override;

	// Status
	bool is_rdy() const { return ready; }

	// Signals
	void reset();
	void clock();
};
