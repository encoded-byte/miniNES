#pragma once

#include <string>
#include "devices/APU.h"
#include "devices/Bus.h"
#include "devices/Cart.h"
#include "devices/DMA.h"
#include "devices/Input.h"
#include "devices/PPU.h"
#include "devices/PRAM.h"
#include "devices/RAM.h"
#include "machine/CPU.h"


class Machine
{
private:
	static const uint16_t frame_length {1789800 / 60};

	Bus   cpu_bus;
	Bus   ppu_bus;
	Cart  cart;
	Input inp;
	RAM   ram;
	PRAM  pal;
	CPU   cpu {cpu_bus};
	APU   apu {cpu_bus};
	PPU   ppu {ppu_bus};
	DMA   dma {cpu_bus, ppu};

public:
	Machine();
	void load(const std::string &filename);
	void change(uint8_t disk) { cart.change(disk); }

	// IO
	void set_input(uint8_t index, uint8_t input) { inp.set_input(index, input); }
	const uint32_t* get_video() const { return ppu.get_output(); }
	const int16_t* get_audio() const { return apu.get_output(); }

	// Signals
	void reset();
	void frame();
};
