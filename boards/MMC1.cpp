#include <algorithm>
#include "boards/MMC1.h"


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void MMC1::reset()
{
	shift = 0x10;
	mirroring = 3;
	prg_mode = 3;
	chr_mode = 1;
	prg_bank = 0;
	chr_bank[0] = 0;
	chr_bank[1] = 0;
	ram_enable = 0;
	prg_offset = 1;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                RAM ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// RAM read: 0x6000 - 0x7fff
uint8_t MMC1::read_ram(uint16_t addr)
{
	if (info.ram_size && ram_enable)
	{
		uint32_t bank_offset = (chr_bank[0] & 0xfc) << 11;
		uint32_t ram_addr = (addr & 0x1fff) | bank_offset;
		ram_addr &= info.ram_size - 1;

		return ram[ram_addr];
	}

	return 0xff;
}

// RAM read: 0x6000 - 0x7fff
uint8_t MMC1A::read_ram(uint16_t addr)
{
	return ram[addr & 0x1fff];
}

// RAM write: 0x6000 - 0x7fff
void MMC1::write_ram(uint16_t addr, uint8_t data)
{
	if (info.ram_size && ram_enable)
	{
		uint32_t bank_offset = (chr_bank[0] & 0xfc) << 11;
		uint32_t ram_addr = (addr & 0x1fff) | bank_offset;
		ram_addr &= info.ram_size - 1;

		ram[ram_addr] = data;
	}
}

// RAM write: 0x6000 - 0x7fff
void MMC1A::write_ram(uint16_t addr, uint8_t data)
{
	ram[addr & 0x1fff] = data;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                PRG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// PRG read: 0x8000 - 0xffff
uint8_t MMC1::read_prg(uint16_t addr)
{
	uint32_t bank_offset = 0;

	if (prg_mode == 0 || prg_mode == 1)
	{
		bank_offset = (prg_bank & 0xfe) << 14;
	}
	else if (prg_mode == 2)
	{
		switch (addr & 0xc000)
		{
		case 0x8000: bank_offset = 0; break;
		case 0xc000: bank_offset = prg_bank << 14; break;
		}
	}
	else if (prg_mode == 3)
	{
		switch (addr & 0xc000)
		{
		case 0x8000: bank_offset = prg_bank << 14; break;
		case 0xc000: bank_offset = std::min(info.prg_size, 0x40000U) - 0x4000; break;
		}
	}

	bank_offset |= prg_offset ? 0x40000 : 0;
	uint32_t bank_mask = (prg_mode & 0x02) ? 0x3fff : 0x7fff;
	uint32_t prg_addr = (addr & bank_mask) | bank_offset;
	prg_addr &= info.prg_size - 1;

	return prg[prg_addr];
}

// PRG write: 0x8000 - 0xffff
void MMC1::write_prg(uint16_t addr, uint8_t data)
{
	if (data & 0x80)
	{
		prg_mode = 3;
		shift = 0x10;
		return;
	}

	bool write = shift & 0x01;
	shift = (shift >> 1) | ((data & 0x01) << 4);

	if (write)
	{
		switch (addr & 0xe000)
		{
		case 0x8000:
			mirroring = shift & 0x03;
			prg_mode = (shift >> 2) & 0x03;
			chr_mode = (shift >> 4) & 0x01;
			break;
		case 0xa000:
			chr_bank[0] = shift;
			prg_offset = shift & 0x10;
			break;
		case 0xc000:
			chr_bank[1] = shift;
			break;
		case 0xe000:
			prg_bank = shift & 0x0f;
			ram_enable = ~shift & 0x10;
			break;
		}

		shift = 0x10;
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                                CHR ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// CHR read: 0x0000 - 0x1fff
uint8_t MMC1::read_chr(uint16_t addr)
{
	uint32_t bank_offset = 0;

	if (chr_mode == 0)
		bank_offset = (chr_bank[0] & 0xfe) << 12;
	else
		bank_offset = chr_bank[addr >> 12] << 12;

	uint16_t bank_mask = chr_mode == 0 ? 0x1fff : 0x0fff;
	uint32_t chr_addr = (addr & bank_mask) | bank_offset;
	chr_addr &= info.chr_size ? info.chr_size - 1 : 0x1fff;
	auto &chr_mem = info.chr_size ? chr : chr_ram;

	return chr_mem[chr_addr];
}

// CHR write: 0x0000 - 0x1fff
void MMC1::write_chr(uint16_t addr, uint8_t data)
{
	uint32_t bank_offset = 0;

	if (chr_mode == 0)
		bank_offset = (chr_bank[0] & 0xfe) << 12;
	else
		bank_offset = chr_bank[addr >> 12] << 12;

	uint16_t bank_mask = chr_mode == 0 ? 0x1fff : 0x0fff;
	uint32_t chr_addr = (addr & bank_mask) | bank_offset;
	chr_addr &= 0x1fff;

	chr_ram[chr_addr] = data;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                NT ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// NT read: 0x2000 - 0x3eff
uint8_t MMC1::read_nt(uint16_t addr)
{
	switch (mirroring)
	{
	case 0: addr = addr_nt_single(addr, 0); break;
	case 1: addr = addr_nt_single(addr, 1); break;
	case 2: addr = addr_nt_vertical(addr); break;
	case 3: addr = addr_nt_horizontal(addr); break;
	}

	return nt_ram[addr];
}

// NT write: 0x2000 - 0x3eff
void MMC1::write_nt(uint16_t addr, uint8_t data)
{
	switch (mirroring)
	{
	case 0: addr = addr_nt_single(addr, 0); break;
	case 1: addr = addr_nt_single(addr, 1); break;
	case 2: addr = addr_nt_vertical(addr); break;
	case 3: addr = addr_nt_horizontal(addr); break;
	}

	nt_ram[addr] = data;
}
