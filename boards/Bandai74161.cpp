#include "boards/Bandai74161.h"


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void Bandai74161::reset()
{
	prg_bank = 0;
	chr_bank = 0;
}

// Signal: Reset
void Bandai74161A::reset()
{
	Bandai74161::reset();
	mirroring = 0;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                REG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// REG write: 0x8000 - 0xffff
void Bandai74161::write_reg(uint16_t addr, uint8_t data)
{
	prg_bank = data >> 4;
	chr_bank = data & 0x0f;
}

// REG write: 0x8000 - 0xffff
void Bandai74161A::write_reg(uint16_t addr, uint8_t data)
{
	Bandai74161::write_reg(addr, data);
	mirroring = data >> 7;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                PRG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// PRG read: 0x8000 - 0xffff
uint8_t Bandai74161::read_prg(uint16_t addr)
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
uint8_t Bandai74161::read_chr(uint16_t addr)
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
uint8_t Bandai74161A::read_nt(uint16_t addr)
{
	addr = addr_nt_single(addr, mirroring);

	return nt_ram[addr];
}

// NT write: 0x2000 - 0x3eff
void Bandai74161A::write_nt(uint16_t addr, uint8_t data)
{
	addr = addr_nt_single(addr, mirroring);

	nt_ram[addr] = data;
}
