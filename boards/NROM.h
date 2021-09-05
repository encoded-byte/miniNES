#pragma once

#include "devices/Board.h"


class NROM : public Board
{
protected:
	// RAM Access
	uint8_t read_ram(uint16_t addr) override;
	void write_ram(uint16_t addr, uint8_t data) override;

	// PRG Access
	uint8_t read_prg(uint16_t addr) override;

	// CHR Access
	uint8_t read_chr(uint16_t addr) override;

public:
	NROM(BoardInfo info) : Board(info) {}
};
