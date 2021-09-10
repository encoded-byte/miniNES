#pragma once

#include "devices/Board.h"


class VRC2 : public Board
{
protected:
	uint8_t prg_bank[2];
	uint16_t chr_bank[8];
	uint8_t mirroring;

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
	VRC2(BoardInfo info) : Board(info) {}

	// Signals
	void reset() override;
};


class VRC2A : public VRC2
{
protected:
	// PRG Access
	void write_prg(uint16_t addr, uint8_t data) override;

public:
	VRC2A(BoardInfo info) : VRC2(info) {}
};


class VRC2B : public VRC2A
{
protected:
	// CHR Access
	uint8_t read_chr(uint16_t addr) override;

public:
	VRC2B(BoardInfo info) : VRC2A(info) {}
};


class VRC4 : public VRC2
{
protected:
	bool prg_mode;
	uint8_t irq_counter;
	uint8_t irq_reload;
	bool irq_enable;
	bool irq_repeat;
	bool irq_scanline;
	bool irq_request;
	int sc_counter;

	// PRG Access
	uint8_t read_prg(uint16_t addr) override;
	void write_prg(uint16_t addr, uint8_t data) override;

	// NT Access
	uint8_t read_nt(uint16_t addr) override;
	void write_nt(uint16_t addr, uint8_t data) override;

public:
	VRC4(BoardInfo info) : VRC2(info) {}

	// Status
	bool is_irq() const override { return irq_request; }

	// Signals
	void reset() override;
	void clock() override;
};


class VRC4A : public VRC4
{
protected:
	// PRG Access
	void write_prg(uint16_t addr, uint8_t data) override;

public:
	VRC4A(BoardInfo info) : VRC4(info) {}
};


class VRC4B : public VRC4
{
protected:
	// PRG Access
	void write_prg(uint16_t addr, uint8_t data) override;

public:
	VRC4B(BoardInfo info) : VRC4(info) {}
};


class VRC4C : public VRC4
{
protected:
	// PRG Access
	void write_prg(uint16_t addr, uint8_t data) override;

public:
	VRC4C(BoardInfo info) : VRC4(info) {}
};


class VRC4D : public VRC4
{
protected:
	// PRG Access
	void write_prg(uint16_t addr, uint8_t data) override;

public:
	VRC4D(BoardInfo info) : VRC4(info) {}
};


class VRC4E : public VRC4
{
protected:
	// PRG Access
	void write_prg(uint16_t addr, uint8_t data) override;

public:
	VRC4E(BoardInfo info) : VRC4(info) {}
};
