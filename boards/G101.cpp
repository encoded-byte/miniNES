#include "boards/G101.h"


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void G101::reset()
{
	for (uint8_t i = 0; i != 2; ++i)
		prg_bank[i] = 0;
	for (uint8_t i = 0; i != 8; ++i)
		chr_bank[i] = 0;
	prg_mode = 0;
	mirroring = 0;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                REG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// REG write: 0x8000 - 0xffff
void G101::write_reg(uint16_t addr, uint8_t data)
{
	switch (addr & 0xf000)
	{
	case 0x8000: case 0xa000:
		prg_bank[(addr >> 13) & 0x01] = data & 0x1f;
		break;
	case 0x9000:
		if (info.submapper == 1) break;
		mirroring = data & 0x01;
		prg_mode = (data >> 1) & 0x01;
		break;
	case 0xb000:
		chr_bank[addr & 0x07] = data;
		break;
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                                RAM ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// RAM read: 0x6000 - 0x7fff
uint8_t G101::read_ram(uint16_t addr)
{
	return ram[addr & 0x1fff];
}

// RAM write: 0x6000 - 0x7fff
void G101::write_ram(uint16_t addr, uint8_t data)
{
	ram[addr & 0x1fff] = data;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                PRG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// PRG read: 0x8000 - 0xffff
uint8_t G101::read_prg(uint16_t addr)
{
	uint32_t bank_offset = 0;
	addr ^= prg_mode && (~addr & 0x2000) ? 0x4000 : 0;

	switch (addr & 0xe000)
	{
	case 0x8000: bank_offset = prg_bank[0] << 13; break;
	case 0xa000: bank_offset = prg_bank[1] << 13; break;
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
uint8_t G101::read_chr(uint16_t addr)
{
	uint16_t bank_data = chr_bank[addr >> 10];
	uint32_t chr_addr = (addr & 0x03ff) | bank_data << 10;
	chr_addr &= info.chr_size - 1;

	return chr[chr_addr];
}


//////////////////////////////////////////////////////////////////////////////
//
//                                 NT ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// NT read: 0x2000 - 0x3eff
uint8_t G101::read_nt(uint16_t addr)
{
	if (info.submapper == 1)
		addr = addr_nt_single(addr, 0);
	else if (mirroring)
		addr = addr_nt_horizontal(addr);
	else
		addr = addr_nt_vertical(addr);

	return nt_ram[addr];
}

// NT write: 0x2000 - 0x3eff
void G101::write_nt(uint16_t addr, uint8_t data)
{
	if (info.submapper == 1)
		addr = addr_nt_single(addr, 0);
	else if (mirroring)
		addr = addr_nt_horizontal(addr);
	else
		addr = addr_nt_vertical(addr);

	nt_ram[addr] = data;
}
