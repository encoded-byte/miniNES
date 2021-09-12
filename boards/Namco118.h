#pragma once

#include "devices/Board.h"


class Namco118 : public Board
{
protected:
	uint8_t bank_select;
	uint8_t bank_data[8];

	// PRG Access
	uint8_t read_prg(uint16_t addr) override;
	void write_prg(uint16_t addr, uint8_t data) override;

	// CHR Access
	uint8_t read_chr(uint16_t addr) override;

public:
	Namco118(BoardInfo info) : Board(info) {}

	// Signals
	void reset() override;
};


class Namco118A : public Namco118
{
protected:
	// CHR Access
	uint8_t read_chr(uint16_t addr) override;

public:
	Namco118A(BoardInfo info) : Namco118(info) {}
};


class Namco118B : public Namco118
{
protected:
	// NT Access
	uint8_t read_nt(uint16_t addr) override;
	void write_nt(uint16_t addr, uint8_t data) override;

public:
	Namco118B(BoardInfo info) : Namco118(info) {}
};


class Namco118C : public Namco118
{
protected:
	uint8_t nt_bank;

	// PRG Access
	void write_prg(uint16_t addr, uint8_t data) override;

	// NT Access
	uint8_t read_nt(uint16_t addr) override;
	void write_nt(uint16_t addr, uint8_t data) override;

public:
	Namco118C(BoardInfo info) : Namco118(info) {}

	// Signals
	void reset() override;
};
