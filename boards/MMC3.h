#pragma once

#include "devices/Board.h"


class MMC3 : public Board
{
protected:
	uint8_t bank_select;
	uint8_t bank_data[8];
	uint8_t prg_mode;
	uint8_t chr_mode;
	uint8_t mirroring;
	uint8_t irq_latch;
	uint8_t irq_counter;
	bool irq_reload;
	bool irq_enable;
	bool irq_trigger;
	bool irq_request;
	bool ram_r_enable;
	bool ram_w_enable;

	void check_irq(uint16_t addr);

	// REG Access
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
	MMC3(BoardInfo info) : Board(info) {}

	// Status
	bool is_irq() const override { return irq_request; }

	// Signals
	void reset() override;
};


class MMC3A : public MMC3
{
protected:
	// CHR Access
	uint8_t read_chr(uint16_t addr) override;

public:
	MMC3A(BoardInfo info) : MMC3(info) {}
};


class MMC3B : public MMC3
{
protected:
	// NT Access
	uint8_t read_nt(uint16_t addr) override;
	void write_nt(uint16_t addr, uint8_t data) override;

public:
	MMC3B(BoardInfo info) : MMC3(info) {}
};
