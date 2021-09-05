#include <stdexcept>
#include "boards/FDS.h"


FDS::FDS() : Board()
{
	std::ifstream ifs("fdsbios.bin", std::ifstream::binary);
	if (!ifs)
		throw std::runtime_error("FDS: Failed to open fdsbios.bin");

	prg.resize(0x2000);
	ifs.read(reinterpret_cast<char*>(prg.data()), prg.size());
}

void FDS::load(std::ifstream &ifs)
{
	for (auto &disk : disks)
		disk.clear();

	for (uint8_t i = 0; !ifs.eof(); ifs.peek(), ++i)
	{
		std::vector<uint8_t> raw(65500);
		ifs.read(reinterpret_cast<char*>(raw.data()), raw.size());

		// Block 1
		auto block_begin = raw.begin();
		auto block_end = block_begin + 56;
		disks[i].insert(disks[i].end(), 3536, 0);
		disks[i].insert(disks[i].end(), 1, 0x80);
		disks[i].insert(disks[i].end(), block_begin, block_end);
		disks[i].insert(disks[i].end(), 2, 0);

		// Block 2
		block_begin = block_end;
		block_end = block_begin + 2;
		disks[i].insert(disks[i].end(), 121, 0);
		disks[i].insert(disks[i].end(), 1, 0x80);
		disks[i].insert(disks[i].end(), block_begin, block_end);
		disks[i].insert(disks[i].end(), 2, 0);

		while (block_end != raw.end() && *block_end == 0x03)
		{
			// Block 3
			block_begin = block_end;
			block_end = block_begin + 16;
			disks[i].insert(disks[i].end(), 121, 0);
			disks[i].insert(disks[i].end(), 1, 0x80);
			disks[i].insert(disks[i].end(), block_begin, block_end);
			disks[i].insert(disks[i].end(), 2, 0);

			// Block 4
			uint16_t block_size = block_begin[13] | block_begin[14] << 8;
			block_begin = block_end;
			block_end = block_begin + block_size + 1;
			disks[i].insert(disks[i].end(), 121, 0);
			disks[i].insert(disks[i].end(), 1, 0x80);
			disks[i].insert(disks[i].end(), block_begin, block_end);
			disks[i].insert(disks[i].end(), 2, 0);
		}

		disks[i].insert(disks[i].end(), 122, 0);
	}
}

void FDS::change(uint8_t disk)
{
	if (!disks[disk].empty())
	{
		drive.disk = disk;
		drive.empty = 1;
		drive.offset = 0;
		drive.scanning = 0;
		drive.gap = 1;
		drive.wait = 895000;
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                             DEVICE INTERFACE
//
//////////////////////////////////////////////////////////////////////////////


// Device: Read
uint8_t FDS::read(uint16_t addr)
{
	uint8_t data = 0;

	if (addr >= 0x0000 && addr <= 0x1fff)
		data = read_chr(addr);
	else if (addr >= 0x2000 && addr <= 0x3eff)
		data = read_nt(addr);
	else if (addr >= 0x4020 && addr <= 0x5fff)
		data = read_reg(addr);
	else if (addr >= 0x6000 && addr <= 0xdfff)
		data = read_ram(addr);
	else if (addr >= 0xe000 && addr <= 0xffff)
		data = read_prg(addr);

	return data;
}

// Device: Write
void FDS::write(uint16_t addr, uint8_t data)
{
	if (addr >= 0x0000 && addr <= 0x1fff)
		write_chr(addr, data);
	else if (addr >= 0x2000 && addr <= 0x3eff)
		write_nt(addr, data);
	else if (addr >= 0x4020 && addr <= 0x5fff)
		write_reg(addr, data);
	else if (addr >= 0x6000 && addr <= 0xdfff)
		write_ram(addr, data);
	else if (addr >= 0xe000 && addr <= 0xffff)
		write_prg(addr, data);
}


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void FDS::reset()
{
	data_reg = 0;
	enabled = 0;
	mirroring = 0;

	timer.reload = 0;
	timer.counter = 0;
	timer.irq = 0;
	timer.enabled = 0;
	timer.repeat = 0;

	drive.offset = 0;
	drive.wait = 0;
	drive.disk = 0;
	drive.power = 0;
	drive.empty = 0;
	drive.ready = 0;
	drive.halt = 0;
	drive.irq = 0;
	drive.irq_enabled = 0;
	drive.read_mode = 0;
	drive.crc = 0;
	drive.transfer = 0;
	drive.scanning = 0;
	drive.gap = 0;
}

// Signal: Clock
void FDS::clock()
{
	if (timer.enabled && --timer.counter == 0)
	{
		timer.irq = 1;
		timer.counter = timer.reload;
		if (!timer.repeat)
			timer.enabled = 0;
	}

	if (!drive.power)
	{
		drive.offset = 0;
		drive.scanning = 0;
		drive.gap = 1;
		drive.wait = 5000;
		return;
	}

	if (drive.halt && !drive.empty && !drive.scanning)
		return;

	if (--drive.wait == 0)
	{
		auto &disk = disks[drive.disk];
		drive.empty = 0;
		drive.scanning = 1;

		if (drive.read_mode)
		{
			if (!drive.ready)
				drive.gap = 1;
			else if (drive.gap)
				drive.gap = disk[drive.offset] == 0;
			else
			{
				data_reg = disk[drive.offset];
				drive.transfer = 1;
				if (drive.irq_enabled)
					drive.irq = 1;
			}
		}
		else
		{
			if (!drive.ready || drive.crc)
				disk[drive.offset] = 0;
			else
			{
				disk[drive.offset] = data_reg;
				drive.transfer = 1;
				if (drive.irq_enabled)
					drive.irq = 1;
			}
		}

		if (++drive.offset == disk.size())
		{
			drive.offset = 0;
			drive.scanning = 0;
			drive.gap = 1;
			drive.wait = 5000;
		}
		else
			drive.wait = 150;
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                                REG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// REG read: 0x4020 - 0x5fff
uint8_t FDS::read_reg(uint16_t addr)
{
	uint8_t data = 0;

	switch (addr)
	{
	case 0x4030:
		if (enabled)
		{
			data = 0x80;
			data |= timer.irq;
			data |= drive.transfer << 1;
			drive.transfer = 0;
		}
		drive.irq = 0;
		timer.irq = 0;
		break;

	case 0x4031:
		if (enabled)
		{
			data = data_reg;
			drive.transfer = 0;
		}
		drive.irq = 0;
		break;

	case 0x4032:
		data = drive.empty;
		data |= !drive.scanning << 1;
		data |= drive.empty << 2;
		break;

	case 0x4033:
		data = 0x80;
		break;
	}

	return data;
}

// REG write: 0x4020 - 0x5fff
void FDS::write_reg(uint16_t addr, uint8_t data)
{
	switch (addr)
	{
	case 0x4020:
		timer.reload = (timer.reload & 0xff00) | data;
		break;

	case 0x4021:
		timer.reload = (timer.reload & 0x00ff) | (data << 8);
		break;

	case 0x4022:
		if (enabled)
		{
			timer.repeat = data & 0x01;
			timer.enabled = data & 0x02;
			if (timer.enabled)
				timer.counter = timer.reload;
			else
				timer.irq = 0;
		}
		break;

	case 0x4023:
		enabled = data & 0x01;
		if (!enabled)
		{
			timer.enabled = 0;
			timer.irq = 0;
		}
		break;

	case 0x4024:
		if (enabled)
		{
			data_reg = data;
			drive.transfer = 0;
		}
		drive.irq = 0;
		break;

	case 0x4025:
		if (enabled)
		{
			drive.power = data & 0x01;
			drive.halt = data & 0x02;
			drive.read_mode = data & 0x04;
			mirroring = data & 0x08;
			drive.crc = data & 0x10;
			drive.ready = data & 0x40;
			drive.irq_enabled = data & 0x80;
		}
		drive.irq = 0;
		break;
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                                RAM ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// RAM read: 0x6000 - 0xdfff
uint8_t FDS::read_ram(uint16_t addr)
{
	return ram[addr & 0x7fff];
}

// RAM write: 0x6000 - 0xdfff
void FDS::write_ram(uint16_t addr, uint8_t data)
{
	ram[addr & 0x7fff] = data;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                PRG ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// PRG read: 0xe000 - 0xffff
uint8_t FDS::read_prg(uint16_t addr)
{
	return prg[addr & 0x1fff];
}


//////////////////////////////////////////////////////////////////////////////
//
//                                CHR ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// CHR read: 0x0000 - 0x1fff
uint8_t FDS::read_chr(uint16_t addr)
{
	return chr_ram[addr];
}

// CHR write: 0x0000 - 0x1fff
void FDS::write_chr(uint16_t addr, uint8_t data)
{
	chr_ram[addr] = data;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                NT ACCESS
//
//////////////////////////////////////////////////////////////////////////////


// NT read: 0x2000 - 0x3eff
uint8_t FDS::read_nt(uint16_t addr)
{
	if (mirroring)
		addr = addr_nt_horizontal(addr);
	else
		addr = addr_nt_vertical(addr);

	return nt_ram[addr];
}

// NT write: 0x2000 - 0x3eff
void FDS::write_nt(uint16_t addr, uint8_t data)
{
	if (mirroring)
		addr = addr_nt_horizontal(addr);
	else
		addr = addr_nt_vertical(addr);

	nt_ram[addr] = data;
}
