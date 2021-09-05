#pragma once

#include <cmath>
#include <fstream>
#include <vector>
#include "machine/Device.h"


struct BoardInfo
{
	const unsigned mapper {0};
	const unsigned submapper {0};
	const unsigned prg_size {0};
	const unsigned chr_size {0};
	const unsigned ram_size {0};
	const unsigned chr_ram_size {0};
	const unsigned nt_ram_size {0};
	const bool mirroring {0};

	BoardInfo() {}
	BoardInfo(uint8_t byte[]) :
		mapper(((byte[8] & 0x0f) << 8) | (byte[7] & 0xf0) | (byte[6] >> 4)),
		submapper(byte[8] >> 4),
		prg_size((byte[9] & 0x0f) != 0x0f
			? (((byte[9] & 0x0f) << 8) | byte[4]) * 0x4000
			: (((byte[4] & 0x03) << 1) | 1) * static_cast<int>(std::pow(2, byte[4] >> 2))),
		chr_size((byte[9] >> 4) != 0x0f
			? (((byte[9] >> 4) << 8) | byte[5]) * 0x2000
			: (((byte[5] & 0x03) << 1) | 1) * static_cast<int>(std::pow(2, byte[5] >> 2))),
		ram_size((byte[10] & 0x0f ? 0x40 << (byte[10] & 0x0f) : 0)
			+ (byte[10] >> 4 ? 0x40 << (byte[10] >> 4) : 0)),
		chr_ram_size(byte[11] & 0x0f ? 0x40 << (byte[11] & 0x0f) : 0),
		nt_ram_size(byte[6] & 0x08 ? 0x1000 : 0x0800),
		mirroring(byte[6] & 0x01)
	{}
};

class Board : public Device
{
protected:
	BoardInfo info;
	std::vector<uint8_t> prg;
	std::vector<uint8_t> chr;
	std::vector<uint8_t> ram;
	std::vector<uint8_t> chr_ram;
	std::vector<uint8_t> nt_ram;

	// RAM Access
	virtual uint8_t read_ram(uint16_t addr) { return 0; }
	virtual void write_ram(uint16_t addr, uint8_t data) {}

	// PRG Access
	virtual uint8_t read_prg(uint16_t addr) = 0;
	virtual void write_prg(uint16_t addr, uint8_t data) {}

	// CHR Access
	virtual uint8_t read_chr(uint16_t addr) = 0;
	virtual void write_chr(uint16_t addr, uint8_t data) {}

	// NT Access
	virtual uint8_t read_nt(uint16_t addr);
	virtual void write_nt(uint16_t addr, uint8_t data);

	// NT Addressing modes
	virtual uint16_t addr_nt_full(uint16_t addr) const final
		{ return addr & 0x0fff; }
	virtual uint16_t addr_nt_vertical(uint16_t addr) const final
		{ return addr & 0x07ff; }
	virtual uint16_t addr_nt_horizontal(uint16_t addr) const final
		{ return (addr & 0x03ff) | (addr & 0x0800) >> 1; }
	virtual uint16_t addr_nt_single(uint16_t addr, uint8_t table) const final
		{ return (addr & 0x03ff) | (table & 0x03) << 10; }

public:
	Board();
	Board(BoardInfo info);
	virtual ~Board() {}
	virtual void load(std::ifstream &ifs);
	virtual void change(uint8_t disk) {}

	// Device interface
	virtual uint8_t read(uint16_t addr) override;
	virtual void write(uint16_t addr, uint8_t data) override;

	// Status
	virtual bool is_irq() const { return 0; }

	// Signals
	virtual void reset() {}
	virtual void clock() {}
};
