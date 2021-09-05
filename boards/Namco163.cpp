#include "boards/Namco163.h"


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void Namco163::reset()
{
	for (uint8_t i = 0; i != 12; ++i)
		chr_bank[i] = 0;
	for (uint8_t i = 0; i != 3; ++i)
		prg_bank[i] = 0;
	for (uint8_t i = 0; i != 4; ++i)
		ram_enable[i] = 0;
	for (uint8_t i = 0; i != 2; ++i)
		nt_ram_enable[i] = 0;
	irq_counter = 0;
	irq_enable = 0;
	irq_request = 0;
}

// Signal: Clock
void Namco163::clock()
{
	if (irq_enable && irq_counter != 0x7fff && ++irq_counter == 0x7fff)
		irq_request = 1;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                IRQ ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// IRQ read: 0x5000 - 0x5fff
uint8_t Namco163::read_irq(uint16_t addr)
{
	uint8_t data = 0;

	switch (addr & 0xf800)
	{
	case 0x5000: data = irq_counter & 0xff; break;
	case 0x5800: data = irq_counter >> 8; break;
	}

	return data;
}

// IRQ write: 0x5000 - 0x5fff
void Namco163::write_irq(uint16_t addr, uint8_t data)
{
	switch (addr & 0xf800)
	{
	case 0x5000:
		irq_counter = (irq_counter & 0xff00) | data;
		break;
	case 0x5800:
		irq_counter = (irq_counter & 0x00ff) | (data & 0x7f) << 8;
		irq_enable = data & 0x80;
		break;
	}

	irq_request = 0;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                RAM ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// RAM read: 0x6000 - 0x7fff
uint8_t Namco163::read_ram(uint16_t addr)
{
	return ram[addr & 0x1fff];
}

// RAM write: 0x6000 - 0x7fff
void Namco163::write_ram(uint16_t addr, uint8_t data)
{
	if (ram_enable[(addr >> 11) & 0x03])
		ram[addr & 0x1fff] = data;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                PRG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// PRG read: 0x8000 - 0xffff
uint8_t Namco163::read_prg(uint16_t addr)
{
	uint32_t bank_offset = 0;

	switch (addr & 0xe000)
	{
	case 0x8000: bank_offset = prg_bank[0] << 13; break;
	case 0xa000: bank_offset = prg_bank[1] << 13; break;
	case 0xc000: bank_offset = prg_bank[2] << 13; break;
	case 0xe000: bank_offset = info.prg_size - 0x2000; break;
	}

	uint32_t prg_addr = (addr & 0x1fff) | bank_offset;
	prg_addr &= info.prg_size - 1;

	return prg[prg_addr];
}

// PRG write: 0x8000 - 0xffff
void Namco163::write_prg(uint16_t addr, uint8_t data)
{
	switch (addr & 0xf800)
	{
	case 0x8000: case 0x8800: case 0x9000: case 0x9800:
	case 0xa000: case 0xa800: case 0xb000: case 0xb800:
	case 0xc000: case 0xc800: case 0xd000: case 0xd800:
		chr_bank[(addr >> 11) & 0x0f] = data;
		break;
	case 0xe000: case 0xf000:
		prg_bank[(addr >> 11) & 0x03] = data & 0x3f;
		break;
	case 0xe800:
		prg_bank[1] = data & 0x3f;
		nt_ram_enable[0] = ~data & 0x40;
		nt_ram_enable[1] = ~data & 0x80;
		break;
	case 0xf800:
		ram_enable[0] = ~data & 0x01;
		ram_enable[1] = ~data & 0x02;
		ram_enable[2] = ~data & 0x04;
		ram_enable[3] = ~data & 0x08;
		break;
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                                CHR ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// CHR read: 0x0000 - 0x1fff
uint8_t Namco163::read_chr(uint16_t addr)
{
	uint8_t bank_data = chr_bank[(addr >> 10) & 0x0f];

	if (bank_data >= 0xe0 && nt_ram_enable[(addr >> 12) & 0x01])
	{
		addr = addr_nt_single(addr, bank_data & 0x01);
		return nt_ram[addr];
	}

	uint32_t bank_offset = bank_data << 10;
	uint32_t chr_addr = (addr & 0x03ff) | bank_offset;
	chr_addr &= info.chr_size - 1;

	return chr[chr_addr];
}

// CHR write: 0x0000 - 0x1fff
void Namco163::write_chr(uint16_t addr, uint8_t data)
{
	uint8_t bank_data = chr_bank[(addr >> 10) & 0x0f];

	if (bank_data >= 0xe0 && nt_ram_enable[(addr >> 12) & 0x01])
	{
		addr = addr_nt_single(addr, bank_data & 0x01);
		nt_ram[addr] = data;
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                                 NT ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// NT read: 0x2000 - 0x3eff
uint8_t Namco163::read_nt(uint16_t addr)
{
	uint8_t bank_data = chr_bank[(addr >> 10) & 0x0b];

	if (bank_data >= 0xe0)
	{
		addr = addr_nt_single(addr, bank_data & 0x01);
		return nt_ram[addr];
	}

	uint32_t bank_offset = bank_data << 10;
	uint32_t chr_addr = (addr & 0x03ff) | bank_offset;
	chr_addr &= info.chr_size - 1;

	return chr[chr_addr];
}

// NT write: 0x2000 - 0x3eff
void Namco163::write_nt(uint16_t addr, uint8_t data)
{
	uint8_t bank_data = chr_bank[(addr >> 10) & 0x0b];

	if (bank_data >= 0xe0)
	{
		addr = addr_nt_single(addr, bank_data & 0x01);
		nt_ram[addr] = data;
	}
}
