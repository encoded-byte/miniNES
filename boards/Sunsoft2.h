#pragma once

#include "devices/Board.h"


class Sunsoft2 : public Board
{
protected:
	uint8_t prg_bank;
	bool chr_enable;

	// REG Access
	void write_reg(uint16_t addr, uint8_t data) override;

	// PRG Access
	uint8_t read_prg(uint16_t addr) override;

	// CHR Access
	uint8_t read_chr(uint16_t addr) override;
	void write_chr(uint16_t addr, uint8_t data) override;

public:
	Sunsoft2(BoardInfo info) : Board(info) {}

	// Signals
	void reset() override;
};


class Sunsoft2A : public Sunsoft2
{
protected:
	uint8_t chr_bank;
	uint8_t nt_bank;

	// REG Access
	void write_reg(uint16_t addr, uint8_t data) override;

	// CHR Access
	uint8_t read_chr(uint16_t addr) override;
	void write_chr(uint16_t addr, uint8_t data) override {}

	// NT Access
	uint8_t read_nt(uint16_t addr) override;
	void write_nt(uint16_t addr, uint8_t data) override;

public:
	Sunsoft2A(BoardInfo info) : Sunsoft2(info) {}

	// Signals
	void reset() override;
};
