#include "boards/GxROM.h"


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void GxROM::reset()
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
void GxROM::write_reg(uint16_t addr, uint8_t data)
{
	prg_bank = (data >> 4) & 0x03;
	chr_bank = data & 0x03;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                PRG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// PRG read: 0x8000 - 0xffff
uint8_t GxROM::read_prg(uint16_t addr)
{
	uint32_t prg_addr = (addr & 0x7fff) | (prg_bank << 15);
	prg_addr &= info.prg_size - 1;

	return prg[prg_addr];
}


//////////////////////////////////////////////////////////////////////////////
//
//                                CHR ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// CHR read: 0x0000 - 0x1fff
uint8_t GxROM::read_chr(uint16_t addr)
{
	uint32_t chr_addr = addr | (chr_bank << 13);
	chr_addr &= info.chr_size - 1;

	return chr[chr_addr];
}
