#include "devices/RAM.h"


//////////////////////////////////////////////////////////////////////////////
//
//                             DEVICE INTERFACE
//
//////////////////////////////////////////////////////////////////////////////


// Device: Read
uint8_t RAM::read(uint16_t addr)
{
	return mem[addr & 0x07ff];
}

// Device: Write
void RAM::write(uint16_t addr, uint8_t data)
{
	mem[addr & 0x07ff] = data;
}
