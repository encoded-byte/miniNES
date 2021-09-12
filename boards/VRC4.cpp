#include "boards/VRC4.h"


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void VRC4::reset()
{
	for (uint8_t i = 0; i != 2; ++i)
		prg_bank[i] = 0;
	for (uint8_t i = 0; i != 8; ++i)
		chr_bank[i] = 0;
	mirroring = 0;
	prg_mode = 0;
	irq_counter = 0;
	irq_reload = 0;
	irq_enable = 0;
	irq_repeat = 0;
	irq_scanline = 0;
	irq_request = 0;
	sc_counter = 341;
}

// Signal: Clock
void VRC4::clock()
{
	if (is_vrc4() && irq_enable)
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
uint8_t VRC4::read_ram(uint16_t addr)
{
	return ram[addr & 0x1fff];
}

// RAM write: 0x6000 - 0x7fff
void VRC4::write_ram(uint16_t addr, uint8_t data)
{
	ram[addr & 0x1fff] = data;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                PRG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// PRG read: 0x8000 - 0xffff
uint8_t VRC4::read_prg(uint16_t addr)
{
	uint32_t bank_offset = 0;

	if (prg_mode && (addr & 0x2000) == 0)
		addr ^= 0x4000;

	switch (addr & 0xe000)
	{
	case 0x8000: bank_offset = prg_bank[0] << 13; break;
	case 0xa000: bank_offset = prg_bank[1] << 13; break;
	case 0xc000: bank_offset = info.prg_size - 0x4000; break;
	case 0xe000: bank_offset = info.prg_size - 0x2000; break;
	}

	uint32_t prg_addr = (addr & 0x1fff) | bank_offset;
	prg_addr &= info.prg_size - 1;

	return prg[prg_addr];
}

// PRG write: 0x8000 - 0xffff
void VRC4::write_prg(uint16_t addr, uint8_t data)
{
	if (info.mapper == 22 || (info.mapper == 25 && info.submapper != 2))
		addr = (addr & 0xf000) | ((addr << 1) & 0x02) | ((addr >> 1) & 0x01);
	if (info.mapper == 21 && info.submapper != 2)
		addr = (addr & 0xf000) | ((addr >> 1) & 0x02) | ((addr >> 1) & 0x01);
	if (info.mapper == 23 && info.submapper == 2)
		addr = (addr & 0xf000) | ((addr >> 2) & 0x02) | ((addr >> 2) & 0x01);
	if (info.mapper == 25 && info.submapper == 2)
		addr = (addr & 0xf000) | ((addr >> 1) & 0x02) | ((addr >> 3) & 0x01);
	if (info.mapper == 21 && info.submapper == 2)
		addr = (addr & 0xf000) | ((addr >> 6) & 0x02) | ((addr >> 6) & 0x01);

	uint8_t prg_index = (addr >> 13) & 0x01;
	uint8_t chr_index = (((addr + 0x1000) >> 11) & 0x06) | ((addr >> 1) & 0x01);

	switch (addr & 0xf003)
	{
	case 0x8000: case 0x8001: case 0x8002: case 0x8003:
	case 0xa000: case 0xa001: case 0xa002: case 0xa003:
		prg_bank[prg_index] = data & 0x1f;
		break;

	case 0x9000: case 0x9001: case 0x9002: case 0x9003:
		if (is_vrc2())
			mirroring = data & 0x01;
		if (is_vrc4() && (addr & 0xf003) == 0x9000)
			mirroring = data & 0x03;
		if (is_vrc4() && (addr & 0xf003) == 0x9002)
			prg_mode = data & 0x02;
		break;

	case 0xb000: case 0xc000: case 0xd000: case 0xe000:
	case 0xb002: case 0xc002: case 0xd002: case 0xe002:
		chr_bank[chr_index] = (chr_bank[chr_index] & 0xfff0) | (data & 0x0f);
		break;

	case 0xb001: case 0xc001: case 0xd001: case 0xe001:
	case 0xb003: case 0xc003: case 0xd003: case 0xe003:
		chr_bank[chr_index] = (chr_bank[chr_index] & 0x000f) | (data & 0x1f) << 4;
		break;

	case 0xf000:
		irq_reload = (irq_reload & 0xf0) | (data & 0x0f);
		break;

	case 0xf001:
		irq_reload = (irq_reload & 0x0f) | (data & 0x0f) << 4;
		break;

	case 0xf002:
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

	case 0xf003:
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
uint8_t VRC4::read_chr(uint16_t addr)
{
	uint8_t bank_data = chr_bank[addr >> 10];
	if (info.mapper == 22) bank_data >>= 1;
	uint32_t chr_addr = (addr & 0x03ff) | bank_data << 10;
	chr_addr &= info.chr_size - 1;

	return chr[chr_addr];
}


//////////////////////////////////////////////////////////////////////////////
//
//                                 NT ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// NT read: 0x2000 - 0x3eff
uint8_t VRC4::read_nt(uint16_t addr)
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
void VRC4::write_nt(uint16_t addr, uint8_t data)
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
