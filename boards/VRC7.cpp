#include "boards/VRC7.h"


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void VRC7::reset()
{
	for (uint8_t i = 0; i != 3; ++i)
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
void VRC7::clock()
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
//                                RAM ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// RAM read: 0x6000 - 0x7fff
uint8_t VRC7::read_ram(uint16_t addr)
{
	return ram_enable ? ram[addr & 0x1fff] : 0xff;
}

// RAM write: 0x6000 - 0x7fff
void VRC7::write_ram(uint16_t addr, uint8_t data)
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
uint8_t VRC7::read_prg(uint16_t addr)
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
void VRC7::write_prg(uint16_t addr, uint8_t data)
{
	if (info.submapper == 2)
		addr = (addr & 0xf000) | ((addr >> 1) & 0x08);

	uint8_t prg_index = ((addr >> 11) & 0x02) | ((addr >> 3) & 0x01);
	uint8_t chr_index = (((addr + 0x2000) >> 11) & 0x06) | ((addr >> 3) & 0x01);

	switch (addr & 0xf008)
	{
	case 0x8000: case 0x8008: case 0x9000:
		prg_bank[prg_index] = data & 0x3f;
		break;

	case 0xa000: case 0xa008: case 0xb000: case 0xb008:
	case 0xc000: case 0xc008: case 0xd000: case 0xd008:
		chr_bank[chr_index] = data;
		break;

	case 0xe000:
		mirroring = data & 0x03;
		ram_enable = data & 0x80;
		break;

	case 0xe008:
		irq_reload = data;
		break;

	case 0xf000:
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

	case 0xf008:
		irq_request = 0;
		irq_enable = irq_repeat;
		break;
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                                CHR ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// CHR read: 0x0000 - 0x1fff
uint8_t VRC7::read_chr(uint16_t addr)
{
	uint32_t bank_offset = chr_bank[addr >> 10] << 10;
	uint32_t chr_addr = (addr & 0x03ff) | bank_offset;
	chr_addr &= info.chr_size ? info.chr_size - 1 : 0x1fff;
	auto &chr_mem = info.chr_size ? chr : chr_ram;

	return chr_mem[chr_addr];
}

// CHR write: 0x0000 - 0x1fff
void VRC7::write_chr(uint16_t addr, uint8_t data)
{
	uint32_t bank_offset = chr_bank[addr >> 10] << 10;
	uint32_t chr_addr = (addr & 0x03ff) | bank_offset;
	chr_addr &= 0x1fff;

	chr_ram[chr_addr] = data;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                 NT ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// NT read: 0x2000 - 0x3eff
uint8_t VRC7::read_nt(uint16_t addr)
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
void VRC7::write_nt(uint16_t addr, uint8_t data)
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
