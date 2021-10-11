#include "boards/CNROM.h"


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void CNROM::reset()
{
	chr_bank = 0;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                REG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// REG write: 0x8000 - 0xffff
void CNROM::write_reg(uint16_t addr, uint8_t data)
{
	chr_bank = data & 0x03;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                PRG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// PRG read: 0x8000 - 0xffff
uint8_t CNROM::read_prg(uint16_t addr)
{
	uint32_t prg_addr = addr & (info.prg_size - 1);

	return prg[prg_addr];
}


//////////////////////////////////////////////////////////////////////////////
//
//                                CHR ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// CHR read: 0x0000 - 0x1fff
uint8_t CNROM::read_chr(uint16_t addr)
{
	uint32_t chr_addr = addr | (chr_bank << 13);
	chr_addr &= info.chr_size - 1;

	return chr[chr_addr];
}

// CHR read: 0x0000 - 0x1fff
uint8_t CNROMA::read_chr(uint16_t addr)
{
	bool chr_enable = chr_bank != 0x00 && chr_bank != 0x13 && chr_bank != 0xf0 && chr_bank != 0x20;

	switch (info.submapper)
	{
	case 4: chr_enable = chr_bank == 0x20; break;
	case 5: chr_enable = chr_bank == 0x11 || chr_bank == 0x21; break;
	case 6: chr_enable = chr_bank == 0x22 || chr_bank == 0x2a; break;
	case 7: chr_enable = chr_bank == 0x0f || chr_bank == 0x33; break;
	}

	return chr_enable ? chr[addr] : 0xff;
}
