#include "boards/VRC6.h"


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void VRC6::reset()
{
	for (uint8_t i = 0; i != 2; ++i)
		prg_bank[i] = 0;
	for (uint8_t i = 0; i != 8; ++i)
		chr_bank[i] = 0;
	mirroring = 0;
	ram_enable = 0;
	irq_counter = 0;
	irq_reload = 0;
	irq_enable = 0;
	irq_repeat = 0;
	irq_scanline = 0;
	irq_request = 0;
	sc_counter = 341;
}

// Signal: Clock
void VRC6::clock()
{
	if (irq_enable)
	{
		sc_counter -= irq_scanline ? 3 : 0;

		if (!irq_scanline || (irq_scanline && sc_counter <= 0))
		{
			sc_counter += irq_scanline ? 341 : 0;

			if (irq_counter == 0xff)
			{
				irq_counter = irq_reload;
				irq_request = 1;
			}
			else
				++irq_counter;
		}
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                                REG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// REG write: 0x8000 - 0xffff
void VRC6::write_reg(uint16_t addr, uint8_t data)
{
	if (info.mapper == 26)
		addr = (addr & 0xfffc) | ((addr << 1) & 0x02) | ((addr >> 1) & 0x01);

	uint8_t prg_index = (addr >> 14) & 0x01;
	uint8_t chr_index = (((addr + 0x1000) >> 10) & 0x04) | (addr & 0x03);

	switch (addr & 0xf003)
	{
	case 0x8000: case 0x8001: case 0x8002: case 0x8003:
	case 0xc000: case 0xc001: case 0xc002: case 0xc003:
		prg_bank[prg_index] = data & 0x1f;
		break;

	case 0xd000: case 0xd001: case 0xd002: case 0xd003:
	case 0xe000: case 0xe001: case 0xe002: case 0xe003:
		chr_bank[chr_index] = data;
		break;

	case 0xb003:
		mirroring = (data >> 2) & 0x03;
		ram_enable = data & 0x80;
		break;

	case 0xf000:
		irq_reload = data;
		break;

	case 0xf001:
		irq_request = 0;
		irq_repeat = data & 0x01;
		irq_enable = data & 0x02;
		irq_scanline = ~data & 0x04;
		if (irq_enable)
		{
			irq_counter = irq_reload;
			sc_counter = 341;
		}
		break;

	case 0xf002:
		irq_request = 0;
		irq_enable = irq_repeat;
		break;
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                                RAM ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// RAM read: 0x6000 - 0x7fff
uint8_t VRC6::read_ram(uint16_t addr)
{
	return ram_enable ? ram[addr & 0x1fff] : 0xff;
}

// RAM write: 0x6000 - 0x7fff
void VRC6::write_ram(uint16_t addr, uint8_t data)
{
	if (ram_enable)
		ram[addr & 0x1fff] = data;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                PRG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// PRG read: 0x8000 - 0xffff
uint8_t VRC6::read_prg(uint16_t addr)
{
	uint32_t bank_offset = 0;

	switch (addr & 0xe000)
	{
	case 0x8000:
	case 0xa000: bank_offset = prg_bank[0] << 14; break;
	case 0xc000: bank_offset = prg_bank[1] << 13; break;
	case 0xe000: bank_offset = info.prg_size - 0x2000; break;
	}

	uint16_t bank_mask = addr & 0x4000 ? 0x1fff : 0x3fff;
	uint32_t prg_addr = (addr & bank_mask) | bank_offset;
	prg_addr &= info.prg_size - 1;

	return prg[prg_addr];
}


//////////////////////////////////////////////////////////////////////////////
//
//                                CHR ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// CHR read: 0x0000 - 0x1fff
uint8_t VRC6::read_chr(uint16_t addr)
{
	uint32_t bank_offset = chr_bank[addr >> 10] << 10;
	uint32_t chr_addr = (addr & 0x03ff) | bank_offset;
	chr_addr &= info.chr_size - 1;

	return chr[chr_addr];
}


//////////////////////////////////////////////////////////////////////////////
//
//                                 NT ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// NT read: 0x2000 - 0x3eff
uint8_t VRC6::read_nt(uint16_t addr)
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
void VRC6::write_nt(uint16_t addr, uint8_t data)
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
