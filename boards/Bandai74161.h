#pragma once

#include "devices/Board.h"


class Bandai74161 : public Board
{
protected:
	uint8_t prg_bank;
	uint8_t chr_bank;

	// REG Access
	void write_reg(uint16_t addr, uint8_t data) override;

	// PRG Access
	uint8_t read_prg(uint16_t addr) override;

	// CHR Access
	uint8_t read_chr(uint16_t addr) override;

public:
	Bandai74161(BoardInfo info) : Board(info) {}

	// Signals
	void reset() override;
};


class Bandai74161A : public Bandai74161
{
protected:
	uint8_t mirroring;

	// REG Access
	void write_reg(uint16_t addr, uint8_t data) override;

	// NT Access
	uint8_t read_nt(uint16_t addr) override;
	void write_nt(uint16_t addr, uint8_t data) override;

public:
	Bandai74161A(BoardInfo info) : Bandai74161(info) {}

	// Signals
	void reset() override;
};
