#include "boards/Namco340.h"


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void Namco340::reset()
{
	for (uint8_t i = 0; i != 8; ++i)
		chr_bank[i] = 0;
	for (uint8_t i = 0; i != 3; ++i)
		prg_bank[i] = 0;
	mirroring = 0;
	ram_enable = 0;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                REG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// REG write: 0x8000 - 0xffff
void Namco340::write_reg(uint16_t addr, uint8_t data)
{
	switch (addr & 0xf800)
	{
	case 0x8000: case 0x8800: case 0x9000: case 0x9800:
	case 0xa000: case 0xa800: case 0xb000: case 0xb800:
		chr_bank[(addr >> 11) & 0x07] = data;
		break;
	case 0xc000:
		ram_enable = data & 0x01;
		break;
	case 0xe000:
		prg_bank[0] = data & 0x3f;
		mirroring = data >> 6;
		break;
	case 0xe800: case 0xf000:
		prg_bank[(addr >> 11) & 0x03] = data & 0x3f;
		break;
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                                RAM ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// RAM read: 0x6000 - 0x7fff
uint8_t Namco340::read_ram(uint16_t addr)
{
	return ram_enable ? ram[addr & 0x07ff] : 0xff;
}

// RAM write: 0x6000 - 0x7fff
void Namco340::write_ram(uint16_t addr, uint8_t data)
{
	if (ram_enable)
		ram[addr & 0x07ff] = data;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                PRG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// PRG read: 0x8000 - 0xffff
uint8_t Namco340::read_prg(uint16_t addr)
{
	uint32_t bank_offset = 0;

	switch (addr & 0xe000)
	{
	case 0x8000: bank_offset = prg_bank[0] << 13; break;
	case 0xa000: bank_offset = prg_bank[1] << 13; break;
	case 0xc000: bank_offset = prg_bank[2] << 13; break;
	case 0xe000: bank_offset = info.prg_size - 0x2000; break;
	}

	uint32_t prg_addr = (addr & 0x1fff) | bank_offset;
	prg_addr &= info.prg_size - 1;

	return prg[prg_addr];
}


//////////////////////////////////////////////////////////////////////////////
//
//                                CHR ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// CHR read: 0x0000 - 0x1fff
uint8_t Namco340::read_chr(uint16_t addr)
{
	uint32_t bank_offset = chr_bank[addr >> 10] << 10;
	uint32_t chr_addr = (addr & 0x03ff) | bank_offset;
	chr_addr &= info.chr_size - 1;

	return chr[chr_addr];
}


//////////////////////////////////////////////////////////////////////////////
//
//                                NT ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// NT read: 0x2000 - 0x3eff
uint8_t Namco340::read_nt(uint16_t addr)
{
	if (info.submapper != 2)
		return Board::read_nt(addr);

	switch (mirroring)
	{
	case 0: addr = addr_nt_single(addr, 0); break;
	case 1: addr = addr_nt_vertical(addr); break;
	case 2: addr = addr_nt_single(addr, 1); break;
	case 3: addr = addr_nt_horizontal(addr); break;
	}

	return nt_ram[addr];
}

// NT write: 0x2000 - 0x3eff
void Namco340::write_nt(uint16_t addr, uint8_t data)
{
	if (info.submapper != 2)
		return Board::write_nt(addr, data);

	switch (mirroring)
	{
	case 0: addr = addr_nt_single(addr, 0); break;
	case 1: addr = addr_nt_vertical(addr); break;
	case 2: addr = addr_nt_single(addr, 1); break;
	case 3: addr = addr_nt_horizontal(addr); break;
	}

	nt_ram[addr] = data;
}
