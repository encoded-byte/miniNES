#include "devices/Board.h"


Board::Board() :
	ram(0x8000, 0),
	chr_ram(0x2000, 0),
	nt_ram(0x1000, 0)
{
}

Board::Board(BoardInfo info) :
	info(info),
	ram(0x8000, 0),
	chr_ram(0x2000, 0),
	nt_ram(0x1000, 0)
{
}

void Board::load(std::ifstream &ifs)
{
	prg.clear();
	prg.resize(info.prg_size);
	ifs.read(reinterpret_cast<char*>(prg.data()), prg.size());

	chr.clear();
	chr.resize(info.chr_size);
	if (info.chr_size)
		ifs.read(reinterpret_cast<char*>(chr.data()), chr.size());
}


//////////////////////////////////////////////////////////////////////////////
//
//                             DEVICE INTERFACE
//
//////////////////////////////////////////////////////////////////////////////


// Device: Read
uint8_t Board::read(uint16_t addr)
{
	uint8_t data = 0;

	if (addr >= 0x0000 && addr <= 0x1fff)
		data = read_chr(addr);
	else if (addr >= 0x2000 && addr <= 0x3eff)
		data = read_nt(addr);
	else if (addr >= 0x6000 && addr <= 0x7fff)
		data = read_ram(addr);
	else if (addr >= 0x8000 && addr <= 0xffff)
		data = read_prg(addr);

	return data;
}

// Device: Write
void Board::write(uint16_t addr, uint8_t data)
{
	if (addr >= 0x0000 && addr <= 0x1fff)
		write_chr(addr, data);
	else if (addr >= 0x2000 && addr <= 0x3eff)
		write_nt(addr, data);
	else if (addr >= 0x6000 && addr <= 0x7fff)
		write_ram(addr, data);
	else if (addr >= 0x8000 && addr <= 0xffff)
		write_reg(addr, data);
}


//////////////////////////////////////////////////////////////////////////////
//
//                                NT ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// NT read: 0x2000 - 0x3eff
uint8_t Board::read_nt(uint16_t addr)
{
	if (info.nt_ram_size == 0x1000)
		addr = addr_nt_full(addr);
	else if (info.mirroring)
		addr = addr_nt_vertical(addr);
	else
		addr = addr_nt_horizontal(addr);

	return nt_ram[addr];
}

// NT write: 0x2000 - 0x3eff
void Board::write_nt(uint16_t addr, uint8_t data)
{
	if (info.nt_ram_size == 0x1000)
		addr = addr_nt_full(addr);
	else if (info.mirroring)
		addr = addr_nt_vertical(addr);
	else
		addr = addr_nt_horizontal(addr);

	nt_ram[addr] = data;
}
