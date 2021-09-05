#pragma once

#include "devices/Board.h"


class Sunsoft5 : public Board
{
protected:
	uint8_t chr_bank[8];
	uint8_t prg_bank[4];
	uint8_t mirroring;
	uint8_t command;
	uint16_t irq_counter;
	bool irq_enable;
	bool irq_decrement;
	bool irq_request;
	bool ram_select;
	bool ram_enable;

	// RAM Access
	uint8_t read_ram(uint16_t addr) override;
	void write_ram(uint16_t addr, uint8_t data) override;

	// PRG Access
	uint8_t read_prg(uint16_t addr) override;
	void write_prg(uint16_t addr, uint8_t data) override;

	// CHR Access
	uint8_t read_chr(uint16_t addr) override;

	// NT Access
	uint8_t read_nt(uint16_t addr) override;
	void write_nt(uint16_t addr, uint8_t data) override;

public:
	Sunsoft5(BoardInfo info) : Board(info) {}

	// Status
	bool is_irq() const override { return irq_request; }

	// Signals
	void reset() override;
	void clock() override;
};
