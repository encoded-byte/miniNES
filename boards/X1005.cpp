#include "boards/X1005.h"


//////////////////////////////////////////////////////////////////////////////
//
//                             DEVICE INTERFACE
//
//////////////////////////////////////////////////////////////////////////////


// Device: Read
uint8_t X1005::read(uint16_t addr)
{
	uint8_t data = 0;

	if (addr >= 0x0000 && addr <= 0x1fff)
		data = read_chr(addr);
	else if (addr >= 0x2000 && addr <= 0x3eff)
		data = read_nt(addr);
	else if (addr >= 0x7f00 && addr <= 0x7fff)
		data = read_ram(addr);
	else if (addr >= 0x8000 && addr <= 0xffff)
		data = read_prg(addr);

	return data;
}

// Device: Write
void X1005::write(uint16_t addr, uint8_t data)
{
	if (addr >= 0x0000 && addr <= 0x1fff)
		write_chr(addr, data);
	else if (addr >= 0x2000 && addr <= 0x3eff)
		write_nt(addr, data);
	else if (addr >= 0x7ef0 && addr <= 0x7eff)
		write_reg(addr, data);
	else if (addr >= 0x7f00 && addr <= 0x7fff)
		write_ram(addr, data);
}


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void X1005::reset()
{
	for (uint8_t i = 0; i != 3; ++i)
		prg_bank[i] = 0;
	for (uint8_t i = 0; i != 6; ++i)
		chr_bank[i] = 0;
	mirroring = 0;
	ram_enable = 0;
}

// Signal: Reset
void X1005A::reset()
{
	X1005::reset();
	for (uint8_t i = 0; i != 2; ++i)
		mirroring[i] = 0;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                REG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// REG write: 0x7ef0 - 0x7eff
void X1005::write_reg(uint16_t addr, uint8_t data)
{
	switch (addr)
	{
	case 0x7ef0: case 0x7ef1: case 0x7ef2:
	case 0x7ef3: case 0x7ef4: case 0x7ef5:
		chr_bank[addr & 0x07] = data;
		break;
	case 0x7ef6: case 0x7ef7:
		mirroring = data & 0x01;
		break;
	case 0x7ef8: case 0x7ef9:
		ram_enable = data == 0xa3;
		break;
	case 0x7efa: case 0x7efb: case 0x7efc:
	case 0x7efd: case 0x7efe: case 0x7eff:
		prg_bank[((addr >> 1) & 0x03) - 1] = data;
		break;
	}
}

// REG write: 0x7ef0 - 0x7eff
void X1005A::write_reg(uint16_t addr, uint8_t data)
{
	switch (addr)
	{
	case 0x7ef0:
		chr_bank[0] = data & 0x7f;
		mirroring[0] = data >> 7;
		break;
	case 0x7ef1:
		chr_bank[1] = data & 0x7f;
		mirroring[1] = data >> 7;
		break;
	case 0x7ef2: case 0x7ef3: case 0x7ef4: case 0x7ef5:
		chr_bank[addr & 0x07] = data;
		break;
	case 0x7ef8: case 0x7ef9:
		ram_enable = data == 0xa3;
		break;
	case 0x7efa: case 0x7efb: case 0x7efc:
	case 0x7efd: case 0x7efe: case 0x7eff:
		prg_bank[((addr >> 1) & 0x03) - 1] = data;
		break;
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                                RAM ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// RAM read: 0x7f00 - 0x7fff
uint8_t X1005::read_ram(uint16_t addr)
{
	if (ram_enable)
		return ram[addr & 0x7f];

	return 0xff;
}

// RAM write: 0x7f00 - 0x7fff
void X1005::write_ram(uint16_t addr, uint8_t data)
{
	if (ram_enable)
		ram[addr & 0x7f] = data;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                PRG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// PRG read: 0x8000 - 0xffff
uint8_t X1005::read_prg(uint16_t addr)
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
uint8_t X1005::read_chr(uint16_t addr)
{
	uint32_t bank_offset = 0;

	switch (addr & 0x1c00)
	{
	case 0x0000: bank_offset = chr_bank[0] << 10; break;
	case 0x0400: bank_offset = (chr_bank[0] + 1) << 10; break;
	case 0x0800: bank_offset = chr_bank[1] << 10; break;
	case 0x0c00: bank_offset = (chr_bank[1] + 1) << 10; break;
	case 0x1000: bank_offset = chr_bank[2] << 10; break;
	case 0x1400: bank_offset = chr_bank[3] << 10; break;
	case 0x1800: bank_offset = chr_bank[4] << 10; break;
	case 0x1c00: bank_offset = chr_bank[5] << 10; break;
	}

	uint32_t chr_addr = (addr & 0x03ff) | bank_offset;
	chr_addr &= info.chr_size - 1;

	return chr[chr_addr];
}


//////////////////////////////////////////////////////////////////////////////
//
//                                NT ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// NT read: 0x2000 - 0x3eff
uint8_t X1005::read_nt(uint16_t addr)
{
	if (mirroring)
		addr = addr_nt_vertical(addr);
	else
		addr = addr_nt_horizontal(addr);

	return nt_ram[addr];
}

// NT write: 0x2000 - 0x3eff
void X1005::write_nt(uint16_t addr, uint8_t data)
{
	if (mirroring)
		addr = addr_nt_vertical(addr);
	else
		addr = addr_nt_horizontal(addr);

	nt_ram[addr] = data;
}

// NT read: 0x2000 - 0x3eff
uint8_t X1005A::read_nt(uint16_t addr)
{
	if (addr < 0x3000)
		addr = addr_nt_single(addr, mirroring[0]);
	else
		addr = addr_nt_single(addr, mirroring[1]);

	return nt_ram[addr];
}

// NT write: 0x2000 - 0x3eff
void X1005A::write_nt(uint16_t addr, uint8_t data)
{
	if (addr < 0x3000)
		addr = addr_nt_single(addr, mirroring[0]);
	else
		addr = addr_nt_single(addr, mirroring[1]);

	nt_ram[addr] = data;
}
