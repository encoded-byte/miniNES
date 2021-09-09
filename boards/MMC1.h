#pragma once

#include "devices/Board.h"


class MMC1 : public Board
{
protected:
	uint8_t shift;
	uint8_t mirroring;
	uint8_t prg_mode;
	uint8_t prg_bank;
	uint8_t chr_mode;
	uint8_t chr_bank[2];
	uint8_t chr_index;
	bool ram_enable;

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
	MMC1(BoardInfo info) : Board(info) {}

	// Signals
	void reset() override;
};


class MMC1A : public MMC1
{
protected:
	// RAM Access
	uint8_t read_ram(uint16_t addr) override;
	void write_ram(uint16_t addr, uint8_t data) override;

public:
	MMC1A(BoardInfo info) : MMC1(info) {}
};


class MMC1B : public MMC1
{
protected:
	// RAM Access
	uint8_t read_ram(uint16_t addr) override;
	void write_ram(uint16_t addr, uint8_t data) override;

public:
	MMC1B(BoardInfo info) : MMC1(info) {}
};


class MMC1C : public MMC1
{
protected:
	// RAM Access
	uint8_t read_ram(uint16_t addr) override;
	void write_ram(uint16_t addr, uint8_t data) override;

public:
	MMC1C(BoardInfo info) : MMC1(info) {}
};
