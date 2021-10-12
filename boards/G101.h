#pragma once

#include "devices/Board.h"


class G101 : public Board
{
protected:
	uint8_t prg_bank[2];
	uint8_t chr_bank[8];
	uint8_t prg_mode;
	uint8_t mirroring;

	// REG Access
	void write_reg(uint16_t addr, uint8_t data) override;

	// RAM Access
	uint8_t read_ram(uint16_t addr) override;
	void write_ram(uint16_t addr, uint8_t data) override;

	// PRG Access
	uint8_t read_prg(uint16_t addr) override;

	// CHR Access
	uint8_t read_chr(uint16_t addr) override;

	// NT Access
	uint8_t read_nt(uint16_t addr) override;
	void write_nt(uint16_t addr, uint8_t data) override;

public:
	G101(BoardInfo info) : Board(info) {}

	// Signals
	void reset() override;
};
