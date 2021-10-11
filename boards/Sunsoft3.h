#pragma once

#include "devices/Board.h"


class Sunsoft3 : public Board
{
protected:
	uint8_t chr_bank[4];
	uint8_t prg_bank;
	uint8_t mirroring;
	uint16_t irq_counter;
	bool irq_enable;
	bool irq_request;
	bool w_latch;

	// REG Access
	void write_reg(uint16_t addr, uint8_t data) override;

	// PRG Access
	uint8_t read_prg(uint16_t addr) override;

	// CHR Access
	uint8_t read_chr(uint16_t addr) override;

	// NT Access
	uint8_t read_nt(uint16_t addr) override;
	void write_nt(uint16_t addr, uint8_t data) override;

public:
	Sunsoft3(BoardInfo info) : Board(info) {}

	// Status
	bool is_irq() const override { return irq_request; }

	// Signals
	void reset() override;
	void clock() override;
};
