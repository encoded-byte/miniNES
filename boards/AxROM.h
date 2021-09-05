#pragma once

#include "devices/Board.h"


class BNROM : public Board
{
protected:
	uint8_t prg_bank;

	// PRG Access
	uint8_t read_prg(uint16_t addr) override;
	void write_prg(uint16_t addr, uint8_t data) override;

	// CHR Access
	uint8_t read_chr(uint16_t addr) override;
	void write_chr(uint16_t addr, uint8_t data) override;

public:
	BNROM(BoardInfo info) : Board(info) {}

	// Signals
	void reset() override;
};


class AxROM : public BNROM
{
protected:
	uint8_t nt_bank;

	// PRG Access
	void write_prg(uint16_t addr, uint8_t data) override;

	// NT Access
	uint8_t read_nt(uint16_t addr) override;
	void write_nt(uint16_t addr, uint8_t data) override;

public:
	AxROM(BoardInfo info) : BNROM(info) {}

	// Signals
	void reset() override;
};
