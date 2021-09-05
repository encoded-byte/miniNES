#pragma once

#include "devices/Board.h"


class VRC3 : public Board
{
protected:
	uint8_t prg_bank;
	uint16_t irq_counter;
	uint16_t irq_reload;
	bool irq_enable;
	bool irq_repeat;
	bool irq_mode;
	bool irq_request;

	// RAM Access
	uint8_t read_ram(uint16_t addr) override;
	void write_ram(uint16_t addr, uint8_t data) override;

	// PRG Access
	uint8_t read_prg(uint16_t addr) override;
	void write_prg(uint16_t addr, uint8_t data) override;

	// CHR Access
	uint8_t read_chr(uint16_t addr) override;
	void write_chr(uint16_t addr, uint8_t data) override;

public:
	VRC3(BoardInfo info) : Board(info) {}

	// Status
	bool is_irq() const override { return irq_request; }

	// Signals
	void reset() override;
	void clock() override;
};
