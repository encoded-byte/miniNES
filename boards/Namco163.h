#pragma once

#include "devices/Board.h"


class Namco163 : public Board
{
protected:
	uint8_t chr_bank[12];
	uint8_t prg_bank[3];
	bool ram_enable[4];
	bool nt_ram_enable[2];
	uint16_t irq_counter;
	bool irq_enable;
	bool irq_request;

	// REG Access
	uint8_t read_reg(uint16_t addr) override;
	void write_reg(uint16_t addr, uint8_t data) override;

	// RAM Access
	uint8_t read_ram(uint16_t addr) override;
	void write_ram(uint16_t addr, uint8_t data) override;

	// PRG Access
	uint8_t read_prg(uint16_t addr) override;

	// CHR Access
	uint8_t read_chr(uint16_t addr) override;
	void write_chr(uint16_t addr, uint8_t data) override;

	// NT Access
	uint8_t read_nt(uint16_t addr) override;
	void write_nt(uint16_t addr, uint8_t data) override;

public:
	Namco163(BoardInfo info) : Board(info) {}

	// Status
	bool is_irq() const override { return irq_request; }

	// Device interface
	uint8_t read(uint16_t addr) override;
	void write(uint16_t addr, uint8_t data) override;

	// Signals
	void reset() override;
	void clock() override;
};
