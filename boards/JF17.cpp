#include "boards/JF17.h"


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void JF17::reset()
{
	prg_bank = 0;
	chr_bank = 0;
	command = 0;
	bank = 0;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                REG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// REG write: 0x8000 - 0xffff
void JF17::write_reg(uint16_t addr, uint8_t data)
{
	if (command & 0x01)
		chr_bank = bank;
	if (command & 0x02)
		prg_bank = bank;
	command = data >> 6;
	bank = data & 0x0f;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                PRG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// PRG read: 0x8000 - 0xffff
uint8_t JF17::read_prg(uint16_t addr)
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

// PRG read: 0x8000 - 0xffff
uint8_t JF19::read_prg(uint16_t addr)
{
	uint32_t bank_offset = 0;

	switch (addr & 0xc000)
	{
	case 0x8000: bank_offset = 0; break;
	case 0xc000: bank_offset = prg_bank << 14; break;
	}

	uint32_t prg_addr = (addr & 0x3fff) | bank_offset;
	prg_addr &= info.prg_size - 1;

	return prg[prg_addr];
}


//////////////////////////////////////////////////////////////////////////////
//
//                                CHR ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// CHR read: 0x0000 - 0x1fff
uint8_t JF17::read_chr(uint16_t addr)
{
	uint32_t chr_addr = addr | chr_bank << 13;
	chr_addr &= info.chr_size - 1;

	return chr[chr_addr];
}
