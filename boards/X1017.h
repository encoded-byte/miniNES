#pragma once

#include "devices/Board.h"


class X1017 : public Board
{
protected:
	uint8_t prg_bank[3];
	uint8_t chr_bank[6];
	uint8_t chr_mode;
	uint8_t mirroring;
	bool ram_enable[3];

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
	X1017(BoardInfo info) : Board(info) {}

	// Device interface
	uint8_t read(uint16_t addr) override;
	void write(uint16_t addr, uint8_t data) override;

	// Signals
	void reset() override;
};
