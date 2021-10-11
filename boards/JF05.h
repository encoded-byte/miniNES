#pragma once

#include "devices/Board.h"


class JF05 : public Board
{
protected:
	uint8_t chr_bank;

	// REG Access
	void write_reg(uint16_t addr, uint8_t data) override;

	// PRG Access
	uint8_t read_prg(uint16_t addr) override;

	// CHR Access
	uint8_t read_chr(uint16_t addr) override;

public:
	JF05(BoardInfo info) : Board(info) {}

	// Device interface
	uint8_t read(uint16_t addr) override;
	void write(uint16_t addr, uint8_t data) override;

	// Signals
	void reset() override;
};


class JF11 : public JF05
{
protected:
	uint8_t prg_bank;

	// REG Access
	void write_reg(uint16_t addr, uint8_t data) override;

	// PRG Access
	uint8_t read_prg(uint16_t addr) override;

public:
	JF11(BoardInfo info) : JF05(info) {}

	// Signals
	void reset() override;
};


class JF13 : public JF11
{
protected:
	// REG Access
	void write_reg(uint16_t addr, uint8_t data) override;

public:
	JF13(BoardInfo info) : JF11(info) {}
};
