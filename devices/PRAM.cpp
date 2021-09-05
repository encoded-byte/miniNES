#include "devices/PRAM.h"


//////////////////////////////////////////////////////////////////////////////
//
//                             DEVICE INTERFACE
//
//////////////////////////////////////////////////////////////////////////////


// Device: Read
uint8_t PRAM::read(uint16_t addr)
{
	addr &= (addr & 0x03) ? 0x1f : 0x0f;
	return mem[addr];
}

// Device: Write
void PRAM::write(uint16_t addr, uint8_t data)
{
	addr &= (addr & 0x03) ? 0x1f : 0x0f;
	mem[addr] = data;
}
