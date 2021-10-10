#include "boards/X1017.h"


//////////////////////////////////////////////////////////////////////////////
//
//                             DEVICE INTERFACE
//
//////////////////////////////////////////////////////////////////////////////


// Device: Read
uint8_t X1017::read(uint16_t addr)
{
	uint8_t data = 0;

	if (addr >= 0x0000 && addr <= 0x1fff)
		data = read_chr(addr);
	else if (addr >= 0x2000 && addr <= 0x3eff)
		data = read_nt(addr);
	else if (addr >= 0x6000 && addr <= 0x73ff)
		data = read_ram(addr);
	else if (addr >= 0x8000 && addr <= 0xffff)
		data = read_prg(addr);

	return data;
}

// Device: Write
void X1017::write(uint16_t addr, uint8_t data)
{
	if (addr >= 0x0000 && addr <= 0x1fff)
		write_chr(addr, data);
	else if (addr >= 0x2000 && addr <= 0x3eff)
		write_nt(addr, data);
	else if (addr >= 0x6000 && addr <= 0x73ff)
		write_ram(addr, data);
	else if (addr >= 0x7ef0 && addr <= 0x7eff)
		write_prg(addr, data);
}


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void X1017::reset()
{
	for (uint8_t i = 0; i != 3; ++i)
		prg_bank[i] = 0;
	for (uint8_t i = 0; i != 6; ++i)
		chr_bank[i] = 0;
	for (uint8_t i = 0; i != 3; ++i)
		ram_enable[i] = 0;
	chr_mode = 0;
	mirroring = 0;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                RAM ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// RAM read: 0x6000 - 0x73ff
uint8_t X1017::read_ram(uint16_t addr)
{
	if (ram_enable[(addr >> 11) & 0x03])
		return ram[addr & 0x1fff];
	
	return 0xff;
}

// RAM write: 0x6000 - 0x73ff
void X1017::write_ram(uint16_t addr, uint8_t data)
{
	if (ram_enable[(addr >> 11) & 0x03])
		ram[addr & 0x1fff] = data;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                PRG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// PRG read: 0x8000 - 0xffff
uint8_t X1017::read_prg(uint16_t addr)
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

// PRG write: 0x7ef0 - 0x7eff
void X1017::write_prg(uint16_t addr, uint8_t data)
{
	switch (addr)
	{
	case 0x7ef0: case 0x7ef1: case 0x7ef2:
	case 0x7ef3: case 0x7ef4: case 0x7ef5:
		chr_bank[addr & 0x07] = data;
		break;
	case 0x7ef6:
		mirroring = data & 0x01;
		chr_mode = (data >> 1) & 0x01;
		break;
	case 0x7ef7:
		ram_enable[0] = data == 0xca;
		break;
	case 0x7ef8:
		ram_enable[1] = data == 0x69;
		break;
	case 0x7ef9:
		ram_enable[2] = data == 0x84;
		break;
	case 0x7efa: case 0x7efb: case 0x7efc:
		prg_bank[(addr & 0x03) ^ 0x02] = data >> 2;
		break;
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                                CHR ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// CHR read: 0x0000 - 0x1fff
uint8_t X1017::read_chr(uint16_t addr)
{
	uint32_t bank_offset = 0;
	addr ^= chr_mode ? 0x1000 : 0;

	switch (addr & 0x1c00)
	{
	case 0x0000:
	case 0x0400: bank_offset = (chr_bank[0] & 0xfe) << 10; break;
	case 0x0800:
	case 0x0c00: bank_offset = (chr_bank[1] & 0xfe) << 10; break;
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


//////////////////////////////////////////////////////////////////////////////
//
//                                NT ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// NT read: 0x2000 - 0x3eff
uint8_t X1017::read_nt(uint16_t addr)
{
	if (mirroring)
		addr = addr_nt_vertical(addr);
	else
		addr = addr_nt_horizontal(addr);

	return nt_ram[addr];
}

// NT write: 0x2000 - 0x3eff
void X1017::write_nt(uint16_t addr, uint8_t data)
{
	if (mirroring)
		addr = addr_nt_vertical(addr);
	else
		addr = addr_nt_horizontal(addr);

	nt_ram[addr] = data;
}