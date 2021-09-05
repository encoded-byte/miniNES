#pragma once

#include "devices/Board.h"


class UxROM : public Board
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
	UxROM(BoardInfo info) : Board(info) {}

	// Signals
	void reset() override;
};


class UxROMA : public UxROM
{
protected:
	// PRG Access
	void write_prg(uint16_t addr, uint8_t data) override;

public:
	UxROMA(BoardInfo info) : UxROM(info) {}
};


class UxROMB : public UxROM
{
protected:
	// PRG Access
	uint8_t read_prg(uint16_t addr) override;

public:
	UxROMB(BoardInfo info) : UxROM(info) {}
};
