#include "boards/FCG2.h"


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void FCG2::reset()
{
	for (uint8_t i = 0; i != 8; ++i)
		chr_bank[i] = 0;
	prg_bank = 0;
	mirroring = 0;
	irq_counter = 0;
	irq_reload = 0;
	irq_enable = 0;
	irq_request = 0;
}

// Signal: Reset
void FCG2A::reset()
{
	FCG2::reset();
	ram_enable = 0;
}

// Signal: Clock
void FCG2::clock()
{
	if (irq_enable && irq_counter-- == 0)
		irq_request = 1;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                REG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// REG write: 0x8000 - 0xffff
void FCG2::write_reg(uint16_t addr, uint8_t data)
{
	switch (addr & 0x800f)
	{
	case 0x8000: case 0x8001: case 0x8002: case 0x8003:
	case 0x8004: case 0x8005: case 0x8006: case 0x8007:
		chr_bank[addr & 0x07] = data;
		break;
	case 0x8008:
		prg_bank = data & 0x0f;
		break;
	case 0x8009:
		mirroring = data & 0x03;
		break;
	case 0x800a:
		irq_enable = data & 0x01;
		irq_request = 0;
		if (info.submapper == 5)
			irq_counter = irq_reload;
		break;
	case 0x800b:
		irq_reload = (irq_reload & 0xff00) | data;
		if (info.submapper != 5)
			irq_counter = irq_reload;
		break;
	case 0x800c:
		irq_reload = (irq_reload & 0x00ff) | data << 8;
		if (info.submapper != 5)
			irq_counter = irq_reload;
		break;
	}
}

// REG write: 0x8000 - 0xffff
void FCG2A::write_reg(uint16_t addr, uint8_t data)
{
	switch (addr & 0x800f)
	{
	case 0x8000: case 0x8001: case 0x8002: case 0x8003:
		prg_bank = (prg_bank & 0x0f) | (data & 0x01) << 4;
		break;
	case 0x8008:
		prg_bank = (prg_bank & 0xf0) | (data & 0x0f);
		break;
	case 0x8009:
		mirroring = data & 0x03;
		break;
	case 0x800a:
		irq_enable = data & 0x01;
		irq_request = 0;
		irq_counter = irq_reload;
		break;
	case 0x800b:
		irq_reload = (irq_reload & 0xff00) | data;
		break;
	case 0x800c:
		irq_reload = (irq_reload & 0x00ff) | data << 8;
		break;
	case 0x800d:
		ram_enable = data & 0x20;
		break;
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                                RAM ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// RAM write: 0x6000 - 0x7fff
void FCG2::write_ram(uint16_t addr, uint8_t data)
{
	write_reg(addr | 0x8000, data);
}

// RAM read: 0x6000 - 0x7fff
uint8_t FCG2A::read_ram(uint16_t addr)
{
	if (ram_enable)
		return ram[addr & 0x1fff];

	return 0xff;
}

// RAM write: 0x6000 - 0x7fff
void FCG2A::write_ram(uint16_t addr, uint8_t data)
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
uint8_t FCG2::read_prg(uint16_t addr)
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
uint8_t FCG2::read_chr(uint16_t addr)
{
	uint8_t bank_data = chr_bank[addr >> 10];
	uint32_t chr_addr = (addr & 0x03ff) | bank_data << 10;
	chr_addr &= info.chr_size - 1;

	return chr[chr_addr];
}

// CHR read: 0x0000 - 0x1fff
uint8_t FCG2A::read_chr(uint16_t addr)
{
	return chr_ram[addr];
}

// CHR write: 0x0000 - 0x1fff
void FCG2A::write_chr(uint16_t addr, uint8_t data)
{
	chr_ram[addr] = data;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                 NT ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// NT read: 0x2000 - 0x3eff
uint8_t FCG2::read_nt(uint16_t addr)
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
void FCG2::write_nt(uint16_t addr, uint8_t data)
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
