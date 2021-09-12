#include "boards/MMC3.h"


void MMC3::check_irq(uint16_t addr)
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
void MMC3::reset()
{
	bank_select = 0;
	for (uint8_t i = 0; i != 8; ++i)
		bank_data[i] = 0;
	prg_mode = 0;
	chr_mode = 0;
	mirroring = 0;
	irq_latch = 0;
	irq_counter = 0;
	irq_reload = 0;
	irq_enable = 0;
	irq_trigger = 1;
	irq_request = 0;
	ram_r_enable = 0;
	ram_w_enable = 0;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                RAM ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// RAM read: 0x6000 - 0x7fff
uint8_t MMC3::read_ram(uint16_t addr)
{
	if (ram_r_enable || info.submapper == 1)
		return ram[addr & 0x1fff];

	return 0xff;
}

// RAM write: 0x6000 - 0x7fff
void MMC3::write_ram(uint16_t addr, uint8_t data)
{
	if (ram_w_enable || info.submapper == 1)
		ram[addr & 0x1fff] = data;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                PRG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// PRG read: 0x8000 - 0xffff
uint8_t MMC3::read_prg(uint16_t addr)
{
	uint32_t bank_offset = 0;
	addr ^= prg_mode && (~addr & 0x2000) ? 0x4000 : 0;

	switch (addr & 0xe000)
	{
	case 0x8000: bank_offset = bank_data[6] << 13; break;
	case 0xa000: bank_offset = bank_data[7] << 13; break;
	case 0xc000: bank_offset = info.prg_size - 0x4000; break;
	case 0xe000: bank_offset = info.prg_size - 0x2000; break;
	}

	uint32_t prg_addr = (addr & 0x1fff) | bank_offset;
	prg_addr &= info.prg_size - 1;

	return prg[prg_addr];
}

// PRG write: 0x8000 - 0xffff
void MMC3::write_prg(uint16_t addr, uint8_t data)
{
	switch (addr & 0xe001)
	{
	case 0x8000:
		bank_select = data & 0x07;
		prg_mode = (data >> 6) & 0x01;
		chr_mode = data >> 7;
		break;
	case 0x8001:
		bank_data[bank_select] = data;
		break;
	case 0xa000:
		mirroring = data & 0x01;
		break;
	case 0xa001:
		ram_w_enable = ~data & 0x40;
		ram_r_enable = data & 0x80;
		break;
	case 0xc000:
		irq_latch = data;
		break;
	case 0xc001:
		irq_reload = 1;
		break;
	case 0xe000:
		irq_enable = 0;
		irq_request = 0;
		break;
	case 0xe001:
		irq_enable = 1;
		break;
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                                CHR ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// CHR read: 0x0000 - 0x1fff
uint8_t MMC3::read_chr(uint16_t addr)
{
	check_irq(addr);

	uint32_t bank_offset = 0;
	addr ^= chr_mode ? 0x1000 : 0;

	switch (addr & 0x1c00)
	{
	case 0x0000:
	case 0x0400: bank_offset = (bank_data[0] & 0xfe) << 10; break;
	case 0x0800:
	case 0x0c00: bank_offset = (bank_data[1] & 0xfe) << 10; break;
	case 0x1000: bank_offset = bank_data[2] << 10; break;
	case 0x1400: bank_offset = bank_data[3] << 10; break;
	case 0x1800: bank_offset = bank_data[4] << 10; break;
	case 0x1c00: bank_offset = bank_data[5] << 10; break;
	}

	uint16_t bank_mask = addr & 0x1000 ? 0x03ff : 0x07ff;
	uint32_t chr_addr = (addr & bank_mask) | bank_offset;
	auto &chr_mem = info.chr_size ? chr : chr_ram;
	chr_addr &= chr_mem.size() - 1;

	return chr_mem[chr_addr];
}

// CHR read: 0x0000 - 0x1fff
uint8_t MMC3A::read_chr(uint16_t addr)
{
	check_irq(addr);

	uint32_t bank_offset = 0;
	addr ^= chr_mode ? 0x1000 : 0;

	switch (addr & 0x1c00)
	{
	case 0x0000:
	case 0x0400: bank_offset = (bank_data[0] & 0xfe) << 10; break;
	case 0x0800:
	case 0x0c00: bank_offset = (bank_data[1] & 0xfe) << 10; break;
	case 0x1000: bank_offset = bank_data[2] << 10; break;
	case 0x1400: bank_offset = bank_data[3] << 10; break;
	case 0x1800: bank_offset = bank_data[4] << 10; break;
	case 0x1c00: bank_offset = bank_data[5] << 10; break;
	}

	uint16_t bank_mask = addr & 0x1000 ? 0x03ff : 0x07ff;
	uint32_t chr_addr = (addr & bank_mask) | bank_offset;
	auto &chr_mem = chr_addr & 0x10000 ? chr_ram : chr;
	chr_addr &= chr_mem.size() - 1;

	return chr_mem[chr_addr];
}

// CHR write: 0x0000 - 0x1fff
void MMC3::write_chr(uint16_t addr, uint8_t data)
{
	check_irq(addr);

	uint32_t bank_offset = 0;
	addr ^= chr_mode ? 0x1000 : 0;

	switch (addr & 0x1c00)
	{
	case 0x0000:
	case 0x0400: bank_offset = (bank_data[0] & 0xfe) << 10; break;
	case 0x0800:
	case 0x0c00: bank_offset = (bank_data[1] & 0xfe) << 10; break;
	case 0x1000: bank_offset = bank_data[2] << 10; break;
	case 0x1400: bank_offset = bank_data[3] << 10; break;
	case 0x1800: bank_offset = bank_data[4] << 10; break;
	case 0x1c00: bank_offset = bank_data[5] << 10; break;
	}

	uint16_t bank_mask = addr & 0x1000 ? 0x03ff : 0x07ff;
	uint32_t chr_addr = (addr & bank_mask) | bank_offset;
	chr_addr &= 0x1fff;

	chr_ram[chr_addr] = data;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                NT ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// NT read: 0x2000 - 0x3eff
uint8_t MMC3::read_nt(uint16_t addr)
{
	if (info.nt_ram_size == 0x1000)
		addr = addr_nt_full(addr);
	else if (mirroring)
		addr = addr_nt_horizontal(addr);
	else
		addr = addr_nt_vertical(addr);

	return nt_ram[addr];
}

// NT read: 0x2000 - 0x3eff
uint8_t MMC3B::read_nt(uint16_t addr)
{
	uint8_t table = 0;
	addr = (addr & 0x0fff) ^ (chr_mode ? 0x1000 : 0);

	switch (addr & 0x1c00)
	{
	case 0x0000:
	case 0x0400: table = bank_data[0] >> 7; break;
	case 0x0800:
	case 0x0c00: table = bank_data[1] >> 7; break;
	case 0x1000: table = bank_data[2] >> 7; break;
	case 0x1400: table = bank_data[3] >> 7; break;
	case 0x1800: table = bank_data[4] >> 7; break;
	case 0x1c00: table = bank_data[5] >> 7; break;
	}

	addr = addr_nt_single(addr, table);

	return nt_ram[addr];
}

// NT write: 0x2000 - 0x3eff
void MMC3::write_nt(uint16_t addr, uint8_t data)
{
	if (info.nt_ram_size == 0x1000)
		addr = addr_nt_full(addr);
	else if (mirroring)
		addr = addr_nt_horizontal(addr);
	else
		addr = addr_nt_vertical(addr);

	nt_ram[addr] = data;
}

// NT write: 0x2000 - 0x3eff
void MMC3B::write_nt(uint16_t addr, uint8_t data)
{
	uint8_t table = 0;
	addr = (addr & 0x0fff) ^ (chr_mode ? 0x1000 : 0);

	switch (addr & 0x1c00)
	{
	case 0x0000:
	case 0x0400: table = bank_data[0] >> 7; break;
	case 0x0800:
	case 0x0c00: table = bank_data[1] >> 7; break;
	case 0x1000: table = bank_data[2] >> 7; break;
	case 0x1400: table = bank_data[3] >> 7; break;
	case 0x1800: table = bank_data[4] >> 7; break;
	case 0x1c00: table = bank_data[5] >> 7; break;
	}

	addr = addr_nt_single(addr, table);

	nt_ram[addr] = data;
}
