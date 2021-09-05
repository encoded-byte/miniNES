#pragma once

#include "devices/Board.h"


class Namco175 : public Board
{
protected:
	uint8_t chr_bank[8];
	uint8_t prg_bank[3];
	bool ram_enable;

	// RAM Access
	uint8_t read_ram(uint16_t addr) override;
	void write_ram(uint16_t addr, uint8_t data) override;

	// PRG Access
	uint8_t read_prg(uint16_t addr) override;
	void write_prg(uint16_t addr, uint8_t data) override;

	// CHR Access
	uint8_t read_chr(uint16_t addr) override;

public:
	Namco175(BoardInfo info) : Board(info) {}

	// Signals
	void reset() override;
};


class Namco340 : public Namco175
{
protected:
	uint8_t mirroring;

	// PRG Access
	void write_prg(uint16_t addr, uint8_t data) override;

	// NT Access
	uint8_t read_nt(uint16_t addr) override;
	void write_nt(uint16_t addr, uint8_t data) override;

public:
	Namco340(BoardInfo info) : Namco175(info) {}

	// Signals
	void reset() override;
};
