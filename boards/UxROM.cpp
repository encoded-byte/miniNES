#include "boards/UxROM.h"


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void UxROM::reset()
{
	prg_bank = 0;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                PRG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// PRG read: 0x8000 - 0xffff
uint8_t UxROM::read_prg(uint16_t addr)
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
uint8_t UxROMB::read_prg(uint16_t addr)
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

// PRG write: 0x8000 - 0xffff
void UxROM::write_prg(uint16_t addr, uint8_t data)
{
	prg_bank = data & 0x0f;
}

// PRG write: 0x8000 - 0xffff
void UxROMA::write_prg(uint16_t addr, uint8_t data)
{
	prg_bank = (data >> 2) & 0x0f;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                CHR ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// CHR read: 0x0000 - 0x1fff
uint8_t UxROM::read_chr(uint16_t addr)
{
	return chr_ram[addr];
}

// CHR write: 0x0000 - 0x1fff
void UxROM::write_chr(uint16_t addr, uint8_t data)
{
	chr_ram[addr] = data;
}
