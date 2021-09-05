#pragma once

#include "devices/Board.h"


class MMC2 : public Board
{
protected:
	uint8_t mirroring;
	uint8_t prg_bank;
	uint8_t chr_bank[2][2];
	uint8_t latch[2];

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
	MMC2(BoardInfo info) : Board(info) {}

	// Signals
	void reset() override;
};


class MMC4 : public MMC2
{
protected:
	// PRG Access
	uint8_t read_prg(uint16_t addr) override;

	// CHR Access
	uint8_t read_chr(uint16_t addr) override;

public:
	MMC4(BoardInfo info) : MMC2(info) {}
};
