#include "boards/IF07.h"


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void IF07::reset()
{
	prg_bank = 0;
	mirroring = 0;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                REG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// REG write: 0x8000 - 0xffff
void IF07::write_reg(uint16_t addr, uint8_t data)
{
	prg_bank = data & 0x1f;
	mirroring = data >> 7;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                PRG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// PRG read: 0x8000 - 0xffff
uint8_t IF07::read_prg(uint16_t addr)
{
	uint32_t bank_offset = 0;

	switch (addr & 0xc000)
	{
	case 0x8000: bank_offset = info.prg_size - 0x4000; break;
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
uint8_t IF07::read_chr(uint16_t addr)
{
	return chr_ram[addr];
}

// CHR write: 0x0000 - 0x1fff
void IF07::write_chr(uint16_t addr, uint8_t data)
{
	chr_ram[addr] = data;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                 NT ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// NT read: 0x2000 - 0x3eff
uint8_t IF07::read_nt(uint16_t addr)
{
	if (mirroring)
		addr = addr_nt_vertical(addr);
	else
		addr = addr_nt_horizontal(addr);

	return nt_ram[addr];
}

// NT write: 0x2000 - 0x3eff
void IF07::write_nt(uint16_t addr, uint8_t data)
{
	if (mirroring)
		addr = addr_nt_vertical(addr);
	else
		addr = addr_nt_horizontal(addr);

	nt_ram[addr] = data;
}
