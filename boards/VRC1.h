#pragma once

#include "devices/Board.h"


class VRC1 : public Board
{
protected:
	uint8_t prg_bank[3];
	uint8_t chr_bank[2];
	uint8_t mirroring;

	// PRG Access
	uint8_t read_prg(uint16_t addr) override;
	void write_prg(uint16_t addr, uint8_t data) override;

	// CHR Access
	uint8_t read_chr(uint16_t addr) override;

	// NT Access
	uint8_t read_nt(uint16_t addr) override;
	void write_nt(uint16_t addr, uint8_t data) override;

public:
	VRC1(BoardInfo info) : Board(info) {}

	// Signals
	void reset() override;
};
