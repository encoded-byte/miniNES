#include "boards/JF24.h"


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void JF24::reset()
{
	for (uint8_t i = 0; i != 3; ++i)
		prg_bank[i] = 0;
	for (uint8_t i = 0; i != 8; ++i)
		chr_bank[i] = 0;
	for (uint8_t i = 0; i != 4; ++i)
		irq_reload[i] = 0;
	mirroring = 0;
	irq_counter = 0;
	irq_mask = 0xffff;
	irq_enable = 0;
	irq_request = 0;
	ram_r_enable = 0;
	ram_w_enable = 0;
}

// Signal: Clock
void JF24::clock()
{
	if (irq_enable)
	{
		if ((irq_counter & irq_mask) == 0)
			irq_request = 1;
		irq_counter = (irq_counter & ~irq_mask) | ((irq_counter - 1) & irq_mask);
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                                REG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// REG write: 0x8000 - 0xffff
void JF24::write_reg(uint16_t addr, uint8_t data)
{
	uint8_t prg_index = ((addr >> 11) & 0x02) | ((addr >> 1) & 0x01);
	uint8_t chr_index = (((addr + 0x2000) >> 11) & 0x06) | ((addr >> 1) & 0x01);

	switch (addr & 0xf003)
	{
	case 0x8000: case 0x8002: case 0x9000:
		prg_bank[prg_index] = (prg_bank[prg_index] & 0xf0) | (data & 0x0f);
		break;

	case 0x8001: case 0x8003: case 0x9001:
		prg_bank[prg_index] = (prg_bank[prg_index] & 0x0f) | (data & 0x03) << 4;
		break;

	case 0x9002:
		ram_r_enable = data & 0x01;
		ram_w_enable = data & 0x02;
		break;

	case 0xa000: case 0xa002: case 0xb000: case 0xb002:
	case 0xc000: case 0xc002: case 0xd000: case 0xd002:
		chr_bank[chr_index] = (chr_bank[chr_index] & 0xf0) | (data & 0x0f);
		break;

	case 0xa001: case 0xa003: case 0xb001: case 0xb003:
	case 0xc001: case 0xc003: case 0xd001: case 0xd003:
		chr_bank[chr_index] = (chr_bank[chr_index] & 0x0f) | (data & 0x0f) << 4;
		break;

	case 0xe000: case 0xe001: case 0xe002: case 0xe003:
		irq_reload[addr & 0x03] = data & 0x0f;
		break;

	case 0xf000:
		irq_counter = irq_reload[3] << 12 | irq_reload[2] << 8 | irq_reload[1] << 4 | irq_reload[0];
		irq_request = 0;
		break;

	case 0xf001:
		irq_enable = data & 0x01;
		irq_mask = data & 0x08 ? 0x000f : data & 0x04 ? 0x00ff : data & 0x02 ? 0x0fff : 0xffff;
		irq_request = 0;
		break;

	case 0xf002:
		mirroring = data & 0x03;
		break;
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                                RAM ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// RAM read: 0x6000 - 0x7fff
uint8_t JF24::read_ram(uint16_t addr)
{
	if (ram_r_enable)
		return ram[addr & 0x1fff];

	return 0xff;
}

// RAM write: 0x6000 - 0x7fff
void JF24::write_ram(uint16_t addr, uint8_t data)
{
	if (ram_w_enable)
		ram[addr & 0x1fff] = data;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                PRG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// PRG read: 0x8000 - 0xffff
uint8_t JF24::read_prg(uint16_t addr)
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


//////////////////////////////////////////////////////////////////////////////
//
//                                CHR ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// CHR read: 0x0000 - 0x1fff
uint8_t JF24::read_chr(uint16_t addr)
{
	uint16_t bank_data = chr_bank[addr >> 10];
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
uint8_t JF24::read_nt(uint16_t addr)
{
	switch (mirroring)
	{
	case 0: addr = addr_nt_horizontal(addr); break;
	case 1: addr = addr_nt_vertical(addr); break;
	case 2: addr = addr_nt_single(addr, 0); break;
	case 3: addr = addr_nt_single(addr, 1); break;
	}

	return nt_ram[addr];
}

// NT write: 0x2000 - 0x3eff
void JF24::write_nt(uint16_t addr, uint8_t data)
{
	switch (mirroring)
	{
	case 0: addr = addr_nt_horizontal(addr); break;
	case 1: addr = addr_nt_vertical(addr); break;
	case 2: addr = addr_nt_single(addr, 0); break;
	case 3: addr = addr_nt_single(addr, 1); break;
	}

	nt_ram[addr] = data;
}
