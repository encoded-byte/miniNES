#include "boards/IF09.h"


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void IF09::reset()
{
	prg_bank = 0;
	chr_bank = 0;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                REG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// REG write: 0x8000 - 0xffff
void IF09::write_reg(uint16_t addr, uint8_t data)
{
	prg_bank = data & 0x0f;
	chr_bank = data >> 4;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                PRG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// PRG read: 0x8000 - 0xffff
uint8_t IF09::read_prg(uint16_t addr)
{
	uint32_t prg_addr = (addr & 0x7fff) | prg_bank << 15;
	prg_addr &= info.prg_size - 1;

	return prg[prg_addr];
}


//////////////////////////////////////////////////////////////////////////////
//
//                                CHR ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// CHR read: 0x0000 - 0x1fff
uint8_t IF09::read_chr(uint16_t addr)
{
	if (addr >= 0x0800)
		return chr_ram[addr];

	uint32_t chr_addr = (addr & 0x07ff) | chr_bank << 11;
	chr_addr &= info.chr_size - 1;

	return chr[chr_addr];
}

// CHR read: 0x0000 - 0x1fff
void IF09::write_chr(uint16_t addr, uint8_t data)
{
	if (addr >= 0x0800)
		chr_ram[addr] = data;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                 NT ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// NT read: 0x2000 - 0x3eff
uint8_t IF09::read_nt(uint16_t addr)
{
	addr = addr_nt_full(addr);

	return nt_ram[addr];
}

// NT write: 0x2000 - 0x3eff
void IF09::write_nt(uint16_t addr, uint8_t data)
{
	addr = addr_nt_full(addr);

	nt_ram[addr] = data;
}
