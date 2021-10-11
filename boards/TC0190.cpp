#include "boards/TC0190.h"


void TC0690::check_irq(uint16_t addr)
{
	if ((addr & 0x1000) && !irq_trigger)
	{
		if (irq_counter == 0 || irq_reload)
			irq_counter = irq_latch + 1;
		if (--irq_counter == 0 && irq_enable)
			irq_request = 1;
		irq_reload = 0;
	}
	irq_trigger = addr & 0x1000;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void TC0190::reset()
{
	for (uint8_t i = 0; i != 2; ++i)
		prg_bank[i] = 0;
	for (uint8_t i = 0; i != 6; ++i)
		chr_bank[i] = 0;
	mirroring = 0;
}

// Signal: Reset
void TC0690::reset()
{
	TC0190::reset();
	irq_latch = 0;
	irq_counter = 0;
	irq_reload = 0;
	irq_enable = 0;
	irq_trigger = 1;
	irq_request = 0;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                REG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// REG write: 0x8000 - 0xffff
void TC0190::write_reg(uint16_t addr, uint8_t data)
{
	switch (addr & 0xa003)
	{
	case 0x8000:
		prg_bank[0] = data & 0x3f;
		mirroring = (data >> 6) & 0x01;
		break;
	case 0x8001:
		prg_bank[1] = data & 0x3f;
		break;
	case 0x8002: case 0x8003:
		chr_bank[addr & 0x01] = data;
		break;
	case 0xa000: case 0xa001: case 0xa002: case 0xa003:
		chr_bank[(addr & 0x03) + 2] = data;
		break;
	}
}

// REG write: 0x8000 - 0xffff
void TC0690::write_reg(uint16_t addr, uint8_t data)
{
	switch (addr & 0xe003)
	{
	case 0x8000: case 0x8001:
		prg_bank[addr & 0x01] = data & 0x3f;
		break;
	case 0x8002: case 0x8003:
		chr_bank[addr & 0x01] = data;
		break;
	case 0xa000: case 0xa001: case 0xa002: case 0xa003:
		chr_bank[(addr & 0x03) + 2] = data;
		break;
	case 0xc000:
		irq_latch = data ^ 0xff;
		break;
	case 0xc001:
		irq_reload = 1;
		break;
	case 0xc002:
		irq_enable = 1;
		break;
	case 0xc003:
		irq_enable = 0;
		irq_request = 0;
		break;
	case 0xe000:
		mirroring = (data >> 6) & 0x01;
		break;
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                                PRG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// PRG read: 0x8000 - 0xffff
uint8_t TC0190::read_prg(uint16_t addr)
{
	uint32_t bank_offset = 0;

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


//////////////////////////////////////////////////////////////////////////////
//
//                                CHR ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// CHR read: 0x0000 - 0x1fff
uint8_t TC0190::read_chr(uint16_t addr)
{
	uint32_t bank_offset = 0;

	switch (addr & 0x1c00)
	{
	case 0x0000:
	case 0x0400: bank_offset = chr_bank[0] << 11; break;
	case 0x0800:
	case 0x0c00: bank_offset = chr_bank[1] << 11; break;
	case 0x1000: bank_offset = chr_bank[2] << 10; break;
	case 0x1400: bank_offset = chr_bank[3] << 10; break;
	case 0x1800: bank_offset = chr_bank[4] << 10; break;
	case 0x1c00: bank_offset = chr_bank[5] << 10; break;
	}

	uint16_t bank_mask = addr & 0x1000 ? 0x03ff : 0x07ff;
	uint32_t chr_addr = (addr & bank_mask) | bank_offset;
	chr_addr &= info.chr_size - 1;

	return chr[chr_addr];
}

// CHR read: 0x0000 - 0x1fff
uint8_t TC0690::read_chr(uint16_t addr)
{
	check_irq(addr);

	return TC0190::read_chr(addr);
}


//////////////////////////////////////////////////////////////////////////////
//
//                                NT ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// NT read: 0x2000 - 0x3eff
uint8_t TC0190::read_nt(uint16_t addr)
{
	if (mirroring)
		addr = addr_nt_horizontal(addr);
	else
		addr = addr_nt_vertical(addr);

	return nt_ram[addr];
}

// NT write: 0x2000 - 0x3eff
void TC0190::write_nt(uint16_t addr, uint8_t data)
{
	if (mirroring)
		addr = addr_nt_horizontal(addr);
	else
		addr = addr_nt_vertical(addr);

	nt_ram[addr] = data;
}
