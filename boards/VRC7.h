#pragma once

#include "devices/Board.h"


class VRC7 : public Board
{
protected:
	uint8_t prg_bank[3];
	uint8_t chr_bank[8];
	uint8_t mirroring;
	bool ram_enable;
	uint8_t irq_counter;
	uint8_t irq_reload;
	bool irq_enable;
	bool irq_repeat;
	bool irq_scanline;
	bool irq_request;
	int sc_counter;

	// RAM Access
	uint8_t read_ram(uint16_t addr) override;
	void write_ram(uint16_t addr, uint8_t data) override;

	// PRG Access
	uint8_t read_prg(uint16_t addr) override;
	void write_prg(uint16_t addr, uint8_t data) override;

	// CHR Access
	uint8_t read_chr(uint16_t addr) override;
	void write_chr(uint16_t addr, uint8_t data) override;

	// NT Access
	uint8_t read_nt(uint16_t addr) override;
	void write_nt(uint16_t addr, uint8_t data) override;

public:
	VRC7(BoardInfo info) : Board(info) {}

	// Status
	bool is_irq() const override { return irq_request; }

	// Signals
	void reset() override;
	void clock() override;
};


class VRC7A : public VRC7
{
protected:
	// PRG Access
	void write_prg(uint16_t addr, uint8_t data) override;

public:
	VRC7A(BoardInfo info) : VRC7(info) {}
};
