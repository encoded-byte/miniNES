#pragma once

#include "devices/Board.h"


class Sunsoft4 : public Board
{
protected:
	uint8_t chr_bank[4];
	uint8_t nt_bank[2];
	uint8_t prg_bank;
	uint8_t mirroring;
	bool ram_enable;
	bool chr_enable;

	// RAM Access
	uint8_t read_ram(uint16_t addr) override;
	void write_ram(uint16_t addr, uint8_t data) override;

	// PRG Access
	uint8_t read_prg(uint16_t addr) override;
	void write_prg(uint16_t addr, uint8_t data) override;

	// CHR Access
	uint8_t read_chr(uint16_t addr) override;

	// NT Access
	uint8_t read_nt(uint16_t addr) override;
	void write_nt(uint16_t addr, uint8_t data) override;

public:
	Sunsoft4(BoardInfo info) : Board(info) {}

	// Signals
	void reset() override;
};
