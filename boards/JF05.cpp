#include "boards/JF05.h"


//////////////////////////////////////////////////////////////////////////////
//
//                             DEVICE INTERFACE
//
//////////////////////////////////////////////////////////////////////////////


// Device: Read
uint8_t JF05::read(uint16_t addr)
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
void JF05::write(uint16_t addr, uint8_t data)
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
void JF05::reset()
{
	chr_bank = 0;
}

// Signal: Reset
void JF11::reset()
{
	prg_bank = 0;
	chr_bank = 0;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                REG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// REG write: 0x6000 - 0x7fff
void JF05::write_reg(uint16_t addr, uint8_t data)
{
	chr_bank = ((data << 1) & 0x02) | ((data >> 1) & 0x01);
}

// REG write: 0x6000 - 0x7fff
void JF11::write_reg(uint16_t addr, uint8_t data)
{
	prg_bank = (data >> 4) & 0x03;
	chr_bank = data & 0x0f;
}

// REG write: 0x6000 - 0x7fff
void JF13::write_reg(uint16_t addr, uint8_t data)
{
	if (addr < 0x7000)
	{
		prg_bank = (data >> 4) & 0x03;
		chr_bank = ((data >> 4) & 0x04) | (data & 0x03);
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                                PRG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// PRG read: 0x8000 - 0xffff
uint8_t JF05::read_prg(uint16_t addr)
{
	uint32_t prg_addr = addr & (info.prg_size - 1);

	return prg[prg_addr];
}

// PRG read: 0x8000 - 0xffff
uint8_t JF11::read_prg(uint16_t addr)
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
uint8_t JF05::read_chr(uint16_t addr)
{
	uint32_t chr_addr = addr | chr_bank << 13;
	chr_addr &= info.chr_size - 1;

	return chr[chr_addr];
}
