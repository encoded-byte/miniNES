#pragma once

#include "devices/Board.h"


class FCG2 : public Board
{
protected:
	uint8_t chr_bank[8];
	uint8_t prg_bank;
	uint8_t mirroring;
	uint16_t irq_counter;
	uint16_t irq_reload;
	bool irq_enable;
	bool irq_request;

	// REG Access
	void write_reg(uint16_t addr, uint8_t data) override;

	// RAM Access
	void write_ram(uint16_t addr, uint8_t data) override;

	// PRG Access
	uint8_t read_prg(uint16_t addr) override;

	// CHR Access
	uint8_t read_chr(uint16_t addr) override;

	// NT Access
	uint8_t read_nt(uint16_t addr) override;
	void write_nt(uint16_t addr, uint8_t data) override;

public:
	FCG2(BoardInfo info) : Board(info) {}

	// Status
	bool is_irq() const override { return irq_request; }

	// Signals
	void reset() override;
	void clock() override;
};


class FCG2A : public FCG2
{
protected:
	bool ram_enable;

	// REG Access
	void write_reg(uint16_t addr, uint8_t data) override;

	// RAM Access
	uint8_t read_ram(uint16_t addr) override;
	void write_ram(uint16_t addr, uint8_t data) override;

	// CHR Access
	uint8_t read_chr(uint16_t addr) override;
	void write_chr(uint16_t addr, uint8_t data) override;

public:
	FCG2A(BoardInfo info) : FCG2(info) {}

	// Signals
	void reset() override;
};
