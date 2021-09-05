#include "boards/Sunsoft2.h"


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void Sunsoft2::reset()
{
	prg_bank = 0;
	chr_enable = 0;
}

// Signal: Reset
void Sunsoft2A::reset()
{
	Sunsoft2::reset();
	chr_bank = 0;
	nt_bank = 0;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                PRG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// PRG read: 0x8000 - 0xffff
uint8_t Sunsoft2::read_prg(uint16_t addr)
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

// PRG write: 0x8000 - 0xffff
void Sunsoft2::write_prg(uint16_t addr, uint8_t data)
{
	prg_bank = (data >> 4) & 0x07;
	chr_enable = data & 0x01;
}

// PRG write: 0x8000 - 0xffff
void Sunsoft2A::write_prg(uint16_t addr, uint8_t data)
{
	Sunsoft2::write_prg(addr, data);
	chr_bank = ((data >> 4) & 0x08) | (data & 0x07);
	nt_bank = (data >> 3) & 0x01;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                CHR ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// CHR read: 0x0000 - 0x1fff
uint8_t Sunsoft2::read_chr(uint16_t addr)
{
	return chr_enable ? chr_ram[addr] : 0xff;
}

// CHR read: 0x0000 - 0x1fff
uint8_t Sunsoft2A::read_chr(uint16_t addr)
{
	uint32_t chr_addr = addr | chr_bank << 13;
	chr_addr &= info.chr_size - 1;

	return chr[chr_addr];
}

// CHR write: 0x0000 - 0x1fff
void Sunsoft2::write_chr(uint16_t addr, uint8_t data)
{
	if (chr_enable)
		chr_ram[addr] = data;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                 NT ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// NT read: 0x2000 - 0x3eff
uint8_t Sunsoft2A::read_nt(uint16_t addr)
{
	addr = addr_nt_single(addr, nt_bank);

	return nt_ram[addr];
}

// NT write: 0x2000 - 0x3eff
void Sunsoft2A::write_nt(uint16_t addr, uint8_t data)
{
	addr = addr_nt_single(addr, nt_bank);

	nt_ram[addr] = data;
}
