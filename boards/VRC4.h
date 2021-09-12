#pragma once

#include "devices/Board.h"


class VRC4 : public Board
{
protected:
	uint8_t prg_bank[2];
	uint16_t chr_bank[8];
	uint8_t mirroring;
	bool prg_mode;
	uint8_t irq_counter;
	uint8_t irq_reload;
	bool irq_enable;
	bool irq_repeat;
	bool irq_scanline;
	bool irq_request;
	int sc_counter;

	bool is_vrc2() const { return info.mapper == 22 || info.submapper == 3; }
	bool is_vrc4() const { return !is_vrc2(); }

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
	VRC4(BoardInfo info) : Board(info) {}

	// Status
	bool is_irq() const override { return irq_request; }

	// Signals
	void reset() override;
	void clock() override;
};
