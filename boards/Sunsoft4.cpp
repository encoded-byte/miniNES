#include "boards/Sunsoft4.h"


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void Sunsoft4::reset()
{
	for (uint8_t i = 0; i != 4; ++i)
		chr_bank[i] = 0;
	for (uint8_t i = 0; i != 2; ++i)
		nt_bank[i] = 0;
	prg_bank = 0;
	mirroring = 0;
	ram_enable = 0;
	chr_enable = 0;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                RAM ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// RAM read: 0x6000 - 0x7fff
uint8_t Sunsoft4::read_ram(uint16_t addr)
{
	return ram_enable ? ram[addr & 0x1fff] : 0xff;
}

// RAM write: 0x6000 - 0x7fff
void Sunsoft4::write_ram(uint16_t addr, uint8_t data)
{
	if (ram_enable)
		ram[addr & 0x1fff] = data;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                PRG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// PRG read: 0x8000 - 0xffff
uint8_t Sunsoft4::read_prg(uint16_t addr)
{
	uint32_t bank_offset = 0;

	switch (addr & 0xc000)
	{
	case 0x8000: bank_offset = prg_bank << 14; break;
	case 0xc000: bank_offset = info.prg_size - 0x4000; break;
	}

	uint32_t prg_addr = (addr & 0x3fff) | bank_offset;
	prg_addr &= info.prg_size - 1;

	return prg[prg_addr];
}

// PRG write: 0x8000 - 0xffff
void Sunsoft4::write_prg(uint16_t addr, uint8_t data)
{
	switch (addr & 0xf000)
	{
	case 0x8000: case 0x9000: case 0xa000: case 0xb000:
		chr_bank[(addr >> 12) & 0x03] = data;
		break;
	case 0xc000: case 0xd000:
		nt_bank[(addr >> 12) & 0x01] = (data & 0x7f) | 0x80;
		break;
	case 0xe000:
		mirroring = data & 0x03;
		chr_enable = data & 0x10;
		break;
	case 0xf000:
		prg_bank = data & 0x0f;
		ram_enable = data & 0x10;
		break;
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                                CHR ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// CHR read: 0x0000 - 0x1fff
uint8_t Sunsoft4::read_chr(uint16_t addr)
{
	uint8_t bank = (addr >> 11) & 0x03;
	uint32_t bank_offset = chr_bank[bank] << 11;
	uint32_t chr_addr = (addr & 0x07ff) | bank_offset;
	chr_addr &= info.chr_size - 1;

	return chr[chr_addr];
}


//////////////////////////////////////////////////////////////////////////////
//
//                                NT ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// NT read: 0x2000 - 0x3eff
uint8_t Sunsoft4::read_nt(uint16_t addr)
{

	switch (mirroring)
	{
	case 0: addr = addr_nt_vertical(addr); break;
	case 1: addr = addr_nt_horizontal(addr); break;
	case 2: addr = addr_nt_single(addr, 0); break;
	case 3: addr = addr_nt_single(addr, 1); break;
	}

	if (!chr_enable)
		return nt_ram[addr];

	uint8_t bank = (addr >> 10) & 0x01;
	uint32_t bank_offset = nt_bank[bank] << 10;
	uint32_t chr_addr = (addr & 0x03ff) | bank_offset;
	chr_addr &= info.chr_size - 1;

	return chr[chr_addr];
}

// NT write: 0x2000 - 0x3eff
void Sunsoft4::write_nt(uint16_t addr, uint8_t data)
{
	switch (mirroring)
	{
	case 0: addr = addr_nt_vertical(addr); break;
	case 1: addr = addr_nt_horizontal(addr); break;
	case 2: addr = addr_nt_single(addr, 0); break;
	case 3: addr = addr_nt_single(addr, 1); break;
	}

	if (!chr_enable)
		nt_ram[addr] = data;
}
