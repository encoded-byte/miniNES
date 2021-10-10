#pragma once

#include "devices/Board.h"


class TC0190 : public Board
{
protected:
	uint8_t prg_bank[2];
	uint8_t chr_bank[6];
	uint8_t mirroring;

	// PRG Access
	uint8_t read_prg(uint16_t addr) override;
	void write_prg(uint16_t addr, uint8_t data) override;

	// CHR Access
	uint8_t read_chr(uint16_t addr) override;

	// NT Access
	uint8_t read_nt(uint16_t addr) override;
	void write_nt(uint16_t addr, uint8_t data) override;

public:
	TC0190(BoardInfo info) : Board(info) {}

	// Signals
	void reset() override;
};


class TC0690 : public TC0190
{
protected:
	uint8_t irq_latch;
	uint8_t irq_counter;
	bool irq_reload;
	bool irq_enable;
	bool irq_trigger;
	bool irq_request;

	void check_irq(uint16_t addr);

	// PRG Access
	void write_prg(uint16_t addr, uint8_t data) override;

	// CHR Access
	uint8_t read_chr(uint16_t addr) override;

public:
	TC0690(BoardInfo info) : TC0190(info) {}

	// Status
	bool is_irq() const override { return irq_request; }

	// Signals
	void reset() override;
};
