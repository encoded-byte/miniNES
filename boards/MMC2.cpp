#include "boards/MMC2.h"


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void MMC2::reset()
{
	mirroring = 0;
	prg_bank = 0;
	chr_bank[0][0] = 0;
	chr_bank[0][1] = 0;
	chr_bank[1][0] = 0;
	chr_bank[1][1] = 0;
	latch[0] = 1;
	latch[1] = 1;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                RAM ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// RAM read: 0x6000 - 0x7fff
uint8_t MMC2::read_ram(uint16_t addr)
{
	return ram[addr & 0x1fff];
}

// RAM write: 0x6000 - 0x7fff
void MMC2::write_ram(uint16_t addr, uint8_t data)
{
	ram[addr & 0x1fff] = data;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                PRG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// PRG read: 0x8000 - 0xffff
uint8_t MMC2::read_prg(uint16_t addr)
{
	uint32_t bank_offset = 0;

	switch (addr & 0xe000)
	{
	case 0x8000: bank_offset = prg_bank << 13; break;
	case 0xa000: bank_offset = info.prg_size - 0x6000; break;
	case 0xc000: bank_offset = info.prg_size - 0x4000; break;
	case 0xe000: bank_offset = info.prg_size - 0x2000; break;
	}

	uint32_t prg_addr = (addr & 0x1fff) | bank_offset;
	prg_addr &= info.prg_size - 1;

	return prg[prg_addr];
}

// PRG read: 0x8000 - 0xffff
uint8_t MMC4::read_prg(uint16_t addr)
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
void MMC2::write_prg(uint16_t addr, uint8_t data)
{
	switch (addr & 0xf000)
	{
	case 0xa000: prg_bank = data & 0x0f; break;
	case 0xb000: chr_bank[0][0] = data & 0x1f; break;
	case 0xc000: chr_bank[0][1] = data & 0x1f; break;
	case 0xd000: chr_bank[1][0] = data & 0x1f; break;
	case 0xe000: chr_bank[1][1] = data & 0x1f; break;
	case 0xf000: mirroring = data & 0x01; break;
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                                CHR ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// CHR read: 0x0000 - 0x1fff
uint8_t MMC2::read_chr(uint16_t addr)
{
	uint8_t i = addr >> 12;

	uint32_t bank_offset = chr_bank[i][latch[i]] << 12;
	uint32_t chr_addr = (addr & 0x0fff) | bank_offset;
	chr_addr &= info.chr_size - 1;

	if (addr == 0x0fd8 || (addr >= 0x1fd8 && addr <= 0x1fdf))
		latch[i] = 0;
	else if (addr == 0x0fe8 || (addr >= 0x1fe8 && addr <= 0x1fef))
		latch[i] = 1;

	return chr[chr_addr];
}

// CHR read: 0x0000 - 0x1fff
uint8_t MMC4::read_chr(uint16_t addr)
{
	uint8_t i = addr >> 12;
	addr &= 0x0fff;

	uint32_t bank_offset = chr_bank[i][latch[i]] << 12;
	uint32_t chr_addr = addr | bank_offset;
	chr_addr &= info.chr_size - 1;

	if (addr >= 0x0fd8 && addr <= 0x0fdf)
		latch[i] = 0;
	else if (addr >= 0x0fe8 && addr <= 0x0fef)
		latch[i] = 1;

	return chr[chr_addr];
}


//////////////////////////////////////////////////////////////////////////////
//
//                                NT ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// NT read: 0x2000 - 0x3eff
uint8_t MMC2::read_nt(uint16_t addr)
{
	if (mirroring)
		addr = addr_nt_horizontal(addr);
	else
		addr = addr_nt_vertical(addr);

	return nt_ram[addr];
}

// NT write: 0x2000 - 0x3eff
void MMC2::write_nt(uint16_t addr, uint8_t data)
{
	if (mirroring)
		addr = addr_nt_horizontal(addr);
	else
		addr = addr_nt_vertical(addr);

	nt_ram[addr] = data;
}
