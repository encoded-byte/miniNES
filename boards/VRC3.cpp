#include "boards/VRC3.h"


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void VRC3::reset()
{
	prg_bank = 0;
	irq_counter = 0;
	irq_reload = 0;
	irq_enable = 0;
	irq_repeat = 0;
	irq_mode = 0;
	irq_request = 0;
}

// Signal: Clock
void VRC3::clock()
{
	if (irq_enable)
	{
		if (irq_mode)
		{
			irq_counter = (irq_counter & 0xff00) | ((irq_counter + 1) & 0x00ff);
			if ((irq_counter & 0x00ff) == 0)
			{
				irq_request = 1;
				irq_counter = (irq_counter & 0xff00) | (irq_reload & 0x00ff);
			}
		}
		else if (++irq_counter == 0)
		{
			irq_request = 1;
			irq_counter = irq_reload;
		}
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                                RAM ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// RAM read: 0x6000 - 0x7fff
uint8_t VRC3::read_ram(uint16_t addr)
{
	return ram[addr & 0x1fff];
}

// RAM write: 0x6000 - 0x7fff
void VRC3::write_ram(uint16_t addr, uint8_t data)
{
	ram[addr & 0x1fff] = data;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                PRG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// PRG read: 0x8000 - 0xffff
uint8_t VRC3::read_prg(uint16_t addr)
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
void VRC3::write_prg(uint16_t addr, uint8_t data)
{
	switch (addr & 0xf000)
	{
	case 0x8000:
		irq_reload = (irq_reload & 0xfff0) | (data & 0x0f);
		break;
	case 0x9000:
		irq_reload = (irq_reload & 0xff0f) | (data & 0x0f) << 4;
		break;
	case 0xa000:
		irq_reload = (irq_reload & 0xf0ff) | (data & 0x0f) << 8;
		break;
	case 0xb000:
		irq_reload = (irq_reload & 0x0fff) | (data & 0x0f) << 12;
		break;
	case 0xc000:
		irq_request = 0;
		irq_repeat = data & 0x01;
		irq_enable = data & 0x02;
		irq_mode = data & 0x04;
		if (irq_enable)
			irq_counter = irq_reload;
		break;
	case 0xd000:
		irq_request = 0;
		irq_enable = irq_repeat;
		break;
	case 0xf000:
		prg_bank = data & 0x07;
		break;
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                                CHR ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// CHR read: 0x0000 - 0x1fff
uint8_t VRC3::read_chr(uint16_t addr)
{
	return chr_ram[addr];
}

// CHR write: 0x0000 - 0x1fff
void VRC3::write_chr(uint16_t addr, uint8_t data)
{
	chr_ram[addr] = data;
}
