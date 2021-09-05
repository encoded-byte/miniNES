#pragma once

#include <memory>
#include <string>
#include "devices/Board.h"
#include "machine/Device.h"


class Cart : public Device
{
private:
	std::unique_ptr<Board> board;

public:
	Cart() {}
	void load(const std::string &filename);
	void change(uint8_t disk) { board->change(disk); }

	// Device interface
	uint8_t read(uint16_t addr) override
		{ return board->read(addr); }
	void write(uint16_t addr, uint8_t data) override
		{ board->write(addr, data); }

	// Status
	bool is_irq() const { return board->is_irq(); }

	// Signals
	void reset() { board->reset(); }
	void clock() { board->clock(); }
};
