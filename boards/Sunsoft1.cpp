#include "boards/Sunsoft1.h"


//////////////////////////////////////////////////////////////////////////////
//
//                             DEVICE INTERFACE
//
//////////////////////////////////////////////////////////////////////////////


// Device: Read
uint8_t Sunsoft1::read(uint16_t addr)
{
	uint8_t data = 0;

	if (addr >= 0x0000 && addr <= 0x1fff)
		data = read_chr(addr);
	else if (addr >= 0x2000 && addr <= 0x3eff)
		data = read_nt(addr);
	else if (addr >= 0x8000 && addr <= 0xffff)
		data = read_prg(addr);

	return data;
}

// Device: Write
void Sunsoft1::write(uint16_t addr, uint8_t data)
{
	if (addr >= 0x0000 && addr <= 0x1fff)
		write_chr(addr, data);
	else if (addr >= 0x2000 && addr <= 0x3eff)
		write_nt(addr, data);
	else if (addr >= 0x6000 && addr <= 0x7fff)
		write_reg(addr, data);
}


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void Sunsoft1::reset()
{
	for (uint8_t i = 0; i != 2; ++i)
		chr_bank[i] = 0;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                REG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// REG write: 0x6000 - 0x7fff
void Sunsoft1::write_reg(uint16_t addr, uint8_t data)
{
	chr_bank[0] = data & 0x07;
	chr_bank[1] = ((data >> 4) & 0x03) | 0x04;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                PRG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// PRG read: 0x8000 - 0xffff
uint8_t Sunsoft1::read_prg(uint16_t addr)
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
uint8_t Sunsoft1::read_chr(uint16_t addr)
{
	uint32_t bank_offset = chr_bank[addr >> 12] << 12;
	uint32_t chr_addr = (addr & 0x0fff) | bank_offset;
	chr_addr &= info.chr_size - 1;

	return chr[chr_addr];
}
