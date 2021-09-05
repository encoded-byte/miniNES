#include "boards/VRC1.h"


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void VRC1::reset()
{
	for (uint8_t i = 0; i != 3; ++i)
		prg_bank[i] = 0;
	for (uint8_t i = 0; i != 2; ++i)
		chr_bank[i] = 0;
	mirroring = 0;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                PRG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// PRG read: 0x8000 - 0xffff
uint8_t VRC1::read_prg(uint16_t addr)
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

// PRG write: 0x8000 - 0xffff
void VRC1::write_prg(uint16_t addr, uint8_t data)
{
	switch (addr & 0xf000)
	{
	case 0x8000: case 0xa000: case 0xc000:
		prg_bank[(addr >> 13) & 0x03] = data & 0x0f;
		break;
	case 0x9000:
		mirroring = data & 0x01;
		chr_bank[0] = (chr_bank[0] & 0x0f) | (data & 0x02 ? 0x10 : 0);
		chr_bank[1] = (chr_bank[1] & 0x0f) | (data & 0x04 ? 0x10 : 0);
		break;
	case 0xe000:
		chr_bank[0] = (chr_bank[0] & 0xf0) | (data & 0x0f);
		break;
	case 0xf000:
		chr_bank[1] = (chr_bank[1] & 0xf0) | (data & 0x0f);
		break;
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                                CHR ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// CHR read: 0x0000 - 0x1fff
uint8_t VRC1::read_chr(uint16_t addr)
{
	uint8_t bank = addr >> 12;
	uint32_t bank_offset = chr_bank[bank] << 12;
	uint32_t chr_addr = (addr & 0x0fff) | bank_offset;
	chr_addr &= info.chr_size - 1;

	return chr[chr_addr];
}


//////////////////////////////////////////////////////////////////////////////
//
//                                NT ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// NT read: 0x2000 - 0x3eff
uint8_t VRC1::read_nt(uint16_t addr)
{
	if (mirroring)
		addr = addr_nt_horizontal(addr);
	else
		addr = addr_nt_vertical(addr);

	return nt_ram[addr];
}

// NT write: 0x2000 - 0x3eff
void VRC1::write_nt(uint16_t addr, uint8_t data)
{
	if (mirroring)
		addr = addr_nt_horizontal(addr);
	else
		addr = addr_nt_vertical(addr);

	nt_ram[addr] = data;
}
