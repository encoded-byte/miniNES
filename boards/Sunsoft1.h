#pragma once

#include "devices/Board.h"


class Sunsoft1 : public Board
{
protected:
	uint8_t chr_bank[2];

	// RAM Access
	void write_ram(uint16_t addr, uint8_t data) override;

	// PRG Access
	uint8_t read_prg(uint16_t addr) override;

	// CHR Access
	uint8_t read_chr(uint16_t addr) override;

public:
	Sunsoft1(BoardInfo info) : Board(info) {}

	// Signals
	void reset() override;
};
