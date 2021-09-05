#include "boards/Sunsoft3.h"


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void Sunsoft3::reset()
{
	for (uint8_t i = 0; i != 4; ++i)
		chr_bank[i] = 0;
	prg_bank = 0;
	mirroring = 0;
	irq_counter = 0;
	irq_enable = 0;
	irq_request = 0;
	w_latch = 0;
}

// Signal: Clock
void Sunsoft3::clock()
{
	if (irq_enable && --irq_counter == 0xffff)
	{
		irq_enable = 0;
		irq_request = 1;
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                                PRG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// PRG read: 0x8000 - 0xffff
uint8_t Sunsoft3::read_prg(uint16_t addr)
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

// PRG write: 0x8000 - 0xffff
void Sunsoft3::write_prg(uint16_t addr, uint8_t data)
{
	switch (addr & 0xf800)
	{
	case 0x8000:
		irq_request = 0;
		break;
	case 0x8800: case 0x9800: case 0xa800: case 0xb800:
		chr_bank[(addr - 0x8800) >> 12] = data & 0x3f;
		break;
	case 0xc800:
		irq_counter = w_latch == 0
			? (irq_counter & 0x00ff) | data << 8
			: (irq_counter & 0xff00) | data;
		w_latch = !w_latch;
		break;
	case 0xd800:
		irq_enable = data & 0x10;
		w_latch = 0;
		break;
	case 0xe800:
		mirroring = data & 0x03;
		break;
	case 0xf800:
		prg_bank = data & 0x0f;
		break;
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                                CHR ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// CHR read: 0x0000 - 0x1fff
uint8_t Sunsoft3::read_chr(uint16_t addr)
{
	uint8_t bank = (addr >> 11) & 0x03;
	uint32_t bank_offset = chr_bank[bank] << 11;
	uint32_t chr_addr = (addr & 0x07ff) | bank_offset;
	chr_addr &= info.chr_size - 1;

	return chr[chr_addr];
}


//////////////////////////////////////////////////////////////////////////////
//
//                                NT ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// NT read: 0x2000 - 0x3eff
uint8_t Sunsoft3::read_nt(uint16_t addr)
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
void Sunsoft3::write_nt(uint16_t addr, uint8_t data)
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
