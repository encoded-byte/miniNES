#include "boards/AxROM.h"


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void BNROM::reset()
{
	prg_bank = 0;
}

// Signal: Reset
void AxROM::reset()
{
	prg_bank = 0;
	nt_bank = 0;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                REG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// REG write: 0x8000 - 0xffff
void BNROM::write_reg(uint16_t addr, uint8_t data)
{
	prg_bank = data & 0x03;
}

// REG write: 0x8000 - 0xffff
void AxROM::write_reg(uint16_t addr, uint8_t data)
{
	prg_bank = data & 0x07;
	nt_bank = (data >> 4) & 0x01;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                PRG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// PRG read: 0x8000 - 0xffff
uint8_t BNROM::read_prg(uint16_t addr)
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
uint8_t BNROM::read_chr(uint16_t addr)
{
	return chr_ram[addr];
}

// CHR write: 0x0000 - 0x1fff
void BNROM::write_chr(uint16_t addr, uint8_t data)
{
	chr_ram[addr] = data;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                NT ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// NT read: 0x2000 - 0x3eff
uint8_t AxROM::read_nt(uint16_t addr)
{
	addr = addr_nt_single(addr, nt_bank);

	return nt_ram[addr];
}

// NT write: 0x2000 - 0x3eff
void AxROM::write_nt(uint16_t addr, uint8_t data)
{
	addr = addr_nt_single(addr, nt_bank);

	nt_ram[addr] = data;
}
