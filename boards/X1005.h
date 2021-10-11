#pragma once

#include "devices/Board.h"


class X1005 : public Board
{
protected:
	uint8_t prg_bank[3];
	uint8_t chr_bank[6];
	uint8_t mirroring;
	bool ram_enable;

	// REG Access
	void write_reg(uint16_t addr, uint8_t data) override;

	// RAM Access
	uint8_t read_ram(uint16_t addr) override;
	void write_ram(uint16_t addr, uint8_t data) override;

	// PRG Access
	uint8_t read_prg(uint16_t addr) override;

	// CHR Access
	uint8_t read_chr(uint16_t addr) override;

	// NT Access
	uint8_t read_nt(uint16_t addr) override;
	void write_nt(uint16_t addr, uint8_t data) override;

public:
	X1005(BoardInfo info) : Board(info) {}

	// Device interface
	uint8_t read(uint16_t addr) override;
	void write(uint16_t addr, uint8_t data) override;

	// Signals
	void reset() override;
};


class X1005A : public X1005
{
protected:
	uint8_t mirroring[2];

	// REG Access
	void write_reg(uint16_t addr, uint8_t data) override;

	// NT Access
	uint8_t read_nt(uint16_t addr) override;
	void write_nt(uint16_t addr, uint8_t data) override;

public:
	X1005A(BoardInfo info) : X1005(info) {}

	// Signals
	void reset() override;
};
