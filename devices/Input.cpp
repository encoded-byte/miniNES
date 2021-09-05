#include "devices/Input.h"


//////////////////////////////////////////////////////////////////////////////
//
//                             DEVICE INTERFACE
//
//////////////////////////////////////////////////////////////////////////////


// Device: Read
uint8_t Input::read(uint16_t addr)
{
	uint8_t index = addr & 0x01;
	uint8_t data = buffer[index] & 0x01;
	buffer[index] >>= 1;
	return data | 0x40;
}

// Device: Write
void Input::write(uint16_t addr, uint8_t data)
{
	if (strobe && (data & 0x01) == 0)
	{
		buffer[0] = input[0];
		buffer[1] = input[1];
	}
	strobe = data & 0x01;
}


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void Input::reset()
{
	input[0] = 0;
	input[1] = 0;
	buffer[0] = 0;
	buffer[1] = 0;
	strobe = 0;
}
