#pragma once

#include "devices/Board.h"


class JF17 : public Board
{
protected:
	uint8_t prg_bank;
	uint8_t chr_bank;
	uint8_t command;
	uint8_t bank;

	// REG Access
	void write_reg(uint16_t addr, uint8_t data) override;

	// PRG Access
	uint8_t read_prg(uint16_t addr) override;

	// CHR Access
	uint8_t read_chr(uint16_t addr) override;

public:
	JF17(BoardInfo info) : Board(info) {}

	// Signals
	void reset() override;
};


class JF19 : public JF17
{
protected:
	// PRG Access
	uint8_t read_prg(uint16_t addr) override;

public:
	JF19(BoardInfo info) : JF17(info) {}
};
