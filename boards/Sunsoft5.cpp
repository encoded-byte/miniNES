#include "boards/Sunsoft5.h"


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void Sunsoft5::reset()
{
	for (uint8_t i = 0; i != 8; ++i)
		chr_bank[i] = 0;
	for (uint8_t i = 0; i != 4; ++i)
		prg_bank[i] = 0;
	mirroring = 0;
	command = 0;
	irq_counter = 0;
	irq_enable = 0;
	irq_decrement = 0;
	irq_request = 0;
	ram_select = 0;
	ram_enable = 0;
}

// Signal: Clock
void Sunsoft5::clock()
{
	if (irq_decrement && --irq_counter == 0xffff && irq_enable)
		irq_request = 1;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                REG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// REG write: 0x8000 - 0xffff
void Sunsoft5::write_reg(uint16_t addr, uint8_t data)
{
	if ((addr & 0xe000) == 0x8000)
	{
		command = data & 0x0f;
	}
	else if ((addr & 0xe000) == 0xa000)
	{
		switch (command)
		{
		case 0x00: case 0x01: case 0x02: case 0x03:
		case 0x04: case 0x05: case 0x06: case 0x07:
			chr_bank[command] = data;
			break;
		case 0x08:
			prg_bank[0] = data & 0x3f;
			ram_select = data & 0x40;
			ram_enable = data & 0x80;
			break;
		case 0x09: case 0x0a: case 0x0b:
			prg_bank[command - 8] = data & 0x3f;
			break;
		case 0x0c:
			mirroring = data & 0x03;
			break;
		case 0x0d:
			irq_enable = data & 0x01;
			irq_decrement = data & 0x80;
			irq_request = 0;
			break;
		case 0x0e:
			irq_counter = (irq_counter & 0xff00) | data;
			break;
		case 0x0f:
			irq_counter = (irq_counter & 0x00ff) | data << 8;
			break;
		}
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                                RAM ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// RAM read: 0x6000 - 0x7fff
uint8_t Sunsoft5::read_ram(uint16_t addr)
{
	if (!ram_select)
	{
		uint32_t bank_offset = prg_bank[0] << 13;
		uint32_t prg_addr = (addr & 0x1fff) | bank_offset;
		prg_addr &= info.prg_size - 1;

		return prg[prg_addr];
	}

	return (ram_enable) ? ram[addr & 0x1fff] : 0xff;
}

// RAM write: 0x6000 - 0x7fff
void Sunsoft5::write_ram(uint16_t addr, uint8_t data)
{
	if (ram_select && ram_enable)
		ram[addr & 0x1fff] = data;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                PRG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// PRG read: 0x8000 - 0xffff
uint8_t Sunsoft5::read_prg(uint16_t addr)
{
	uint32_t bank_offset = 0;

	switch (addr & 0xe000)
	{
	case 0x8000: bank_offset = prg_bank[1] << 13; break;
	case 0xa000: bank_offset = prg_bank[2] << 13; break;
	case 0xc000: bank_offset = prg_bank[3] << 13; break;
	case 0xe000: bank_offset = info.prg_size - 0x2000; break;
	}

	uint32_t prg_addr = (addr & 0x1fff) | bank_offset;
	prg_addr &= info.prg_size - 1;

	return prg[prg_addr];
}


//////////////////////////////////////////////////////////////////////////////
//
//                                CHR ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// CHR read: 0x0000 - 0x1fff
uint8_t Sunsoft5::read_chr(uint16_t addr)
{
	uint8_t bank = (addr >> 10) & 0x07;
	uint32_t bank_offset = chr_bank[bank] << 10;
	uint32_t chr_addr = (addr & 0x03ff) | bank_offset;
	chr_addr &= info.chr_size - 1;

	return chr[chr_addr];
}


//////////////////////////////////////////////////////////////////////////////
//
//                                NT ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// NT read: 0x2000 - 0x3eff
uint8_t Sunsoft5::read_nt(uint16_t addr)
{
	switch (mirroring)
	{
	case 0: addr = addr_nt_vertical(addr); break;
	case 1: addr = addr_nt_horizontal(addr); break;
	case 2: addr = addr_nt_single(addr, 0); break;
	case 3: addr = addr_nt_single(addr, 1); break;
	}

	return nt_ram[addr];
}

// NT write: 0x2000 - 0x3eff
void Sunsoft5::write_nt(uint16_t addr, uint8_t data)
{
	switch (mirroring)
	{
	case 0: addr = addr_nt_vertical(addr); break;
	case 1: addr = addr_nt_horizontal(addr); break;
	case 2: addr = addr_nt_single(addr, 0); break;
	case 3: addr = addr_nt_single(addr, 1); break;
	}

	nt_ram[addr] = data;
}
