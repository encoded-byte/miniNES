#pragma once

#include <fstream>
#include <vector>
#include "devices/Board.h"


class FDS : public Board
{
protected:
	std::vector<uint8_t> disks[4];

	struct Timer
	{
		uint16_t reload;
		uint16_t counter;
		bool irq;
		bool enabled;
		bool repeat;
	};

	struct Drive
	{
		uint32_t offset;
		uint32_t wait;
		uint8_t disk;
		bool power;
		bool empty;
		bool ready;
		bool halt;
		bool irq;
		bool irq_enabled;
		bool read_mode;
		bool crc;
		bool transfer;
		bool scanning;
		bool gap;
	};

	// Registers
	Timer timer;
	Drive drive;
	uint8_t data_reg;
	bool enabled;
	bool mirroring;

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
	FDS();
	void load(std::ifstream &ifs) override;
	void change(uint8_t disk) override;

	// Device interface
	uint8_t read(uint16_t addr) override;
	void write(uint16_t addr, uint8_t data) override;

	// Status
	bool is_irq() const override { return drive.irq || timer.irq; }

	// Signals
	void reset() override;
	void clock() override;
};
