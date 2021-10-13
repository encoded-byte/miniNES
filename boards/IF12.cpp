#include "boards/IF12.h"


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void IF12::reset()
{
	prg_bank = 0;
	chr_bank = 0;
	mirroring = 0;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                REG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// REG write: 0x8000 - 0xffff
void IF12::write_reg(uint16_t addr, uint8_t data)
{
	prg_bank = data & 0x07;
	chr_bank = data >> 4;
	mirroring = (data >> 3) & 0x01;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                PRG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// PRG read: 0x8000 - 0xffff
uint8_t IF12::read_prg(uint16_t addr)
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


//////////////////////////////////////////////////////////////////////////////
//
//                                CHR ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// CHR read: 0x0000 - 0x1fff
uint8_t IF12::read_chr(uint16_t addr)
{
	uint32_t chr_addr = addr | chr_bank << 13;
	chr_addr &= info.chr_size - 1;

	return chr[chr_addr];
}


//////////////////////////////////////////////////////////////////////////////
//
//                                 NT ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// NT read: 0x2000 - 0x3eff
uint8_t IF12::read_nt(uint16_t addr)
{
	if (info.submapper != 3)
		addr = addr_nt_single(addr, mirroring);
	else if (mirroring)
		addr = addr_nt_vertical(addr);
	else
		addr = addr_nt_horizontal(addr);

	return nt_ram[addr];
}

// NT write: 0x2000 - 0x3eff
void IF12::write_nt(uint16_t addr, uint8_t data)
{
	if (info.submapper != 3)
		addr = addr_nt_single(addr, mirroring);
	else if (mirroring)
		addr = addr_nt_vertical(addr);
	else
		addr = addr_nt_horizontal(addr);

	nt_ram[addr] = data;
}
