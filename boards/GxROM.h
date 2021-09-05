#pragma once

#include "devices/Board.h"


class GxROM : public Board
{
protected:
	uint8_t prg_bank;
	uint8_t chr_bank;

	// PRG Access
	uint8_t read_prg(uint16_t addr) override;
	void write_prg(uint16_t addr, uint8_t data) override;

	// CHR Access
	uint8_t read_chr(uint16_t addr) override;

public:
	GxROM(BoardInfo info) : Board(info) {}

	// Signals
	void reset() override;
};
