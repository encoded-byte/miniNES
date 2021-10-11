#include "boards/Namco118.h"


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void Namco118::reset()
{
	bank_select = 0;
	for (uint8_t i = 0; i != 8; ++i)
		bank_data[i] = 0;
}

// Signal: Reset
void Namco118C::reset()
{
	Namco118::reset();
	nt_bank = 0;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                REG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// REG write: 0x8000 - 0xffff
void Namco118::write_reg(uint16_t addr, uint8_t data)
{
	switch (addr & 0xe001)
	{
	case 0x8000: bank_select = data & 0x07; break;
	case 0x8001: bank_data[bank_select] = data & 0x3f; break;
	}
}

// REG write: 0x8000 - 0xffff
void Namco118C::write_reg(uint16_t addr, uint8_t data)
{
	Namco118::write_reg(addr, data);
	nt_bank = (data >> 6) & 0x01;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                PRG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// PRG read: 0x8000 - 0xffff
uint8_t Namco118::read_prg(uint16_t addr)
{
	uint32_t bank_offset = 0;

	switch (addr & 0xe000)
	{
	case 0x8000: bank_offset = bank_data[6] << 13; break;
	case 0xa000: bank_offset = bank_data[7] << 13; break;
	case 0xc000: bank_offset = info.prg_size - 0x4000; break;
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
uint8_t Namco118::read_chr(uint16_t addr)
{
	uint32_t bank_offset = 0;

	switch (addr & 0x1c00)
	{
	case 0x0000:
	case 0x0400: bank_offset = (bank_data[0] & 0xfe) << 10; break;
	case 0x0800:
	case 0x0c00: bank_offset = (bank_data[1] & 0xfe) << 10; break;
	case 0x1000: bank_offset = bank_data[2] << 10; break;
	case 0x1400: bank_offset = bank_data[3] << 10; break;
	case 0x1800: bank_offset = bank_data[4] << 10; break;
	case 0x1c00: bank_offset = bank_data[5] << 10; break;
	}

	bank_offset |= addr & 0x1000 ? 0x10000 : 0;
	uint16_t bank_mask = addr & 0x1000 ? 0x03ff : 0x07ff;
	uint32_t chr_addr = (addr & bank_mask) | bank_offset;
	chr_addr &= info.chr_size - 1;

	return chr[chr_addr];
}

// CHR read: 0x0000 - 0x1fff
uint8_t Namco118A::read_chr(uint16_t addr)
{
	uint32_t bank_offset = 0;

	switch (addr & 0x1800)
	{
	case 0x0000: bank_offset = bank_data[2] << 11; break;
	case 0x0800: bank_offset = bank_data[3] << 11; break;
	case 0x1000: bank_offset = bank_data[4] << 11; break;
	case 0x1800: bank_offset = bank_data[5] << 11; break;
	}

	uint32_t chr_addr = (addr & 0x07ff) | bank_offset;
	chr_addr &= info.chr_size - 1;

	return chr[chr_addr];
}


//////////////////////////////////////////////////////////////////////////////
//
//                                 NT ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// NT read: 0x2000 - 0x3eff
uint8_t Namco118B::read_nt(uint16_t addr)
{
	uint8_t bank = (addr >> 11) & 0x01;
	addr = addr_nt_single(addr, (bank_data[bank] >> 5) & 0x01);

	return nt_ram[addr];
}

// NT write: 0x2000 - 0x3eff
void Namco118B::write_nt(uint16_t addr, uint8_t data)
{
	uint8_t bank = (addr >> 11) & 0x01;
	addr = addr_nt_single(addr, (bank_data[bank] >> 5) & 0x01);

	nt_ram[addr] = data;
}

// NT read: 0x2000 - 0x3eff
uint8_t Namco118C::read_nt(uint16_t addr)
{
	addr = addr_nt_single(addr, nt_bank);

	return nt_ram[addr];
}

// NT write: 0x2000 - 0x3eff
void Namco118C::write_nt(uint16_t addr, uint8_t data)
{
	addr = addr_nt_single(addr, nt_bank);

	nt_ram[addr] = data;
}
