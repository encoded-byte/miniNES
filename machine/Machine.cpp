#include "machine/Machine.h"


Machine::Machine()
{
	cpu_bus.add(ram,  0x0000, 0x1fff, 0);
	cpu_bus.add(ppu,  0x2000, 0x3fff, 0);
	cpu_bus.add(apu,  0x4000, 0x4013, 0);
	cpu_bus.add(dma,  0x4014, 0x4014, 0);
	cpu_bus.add(apu,  0x4015, 0x4015, 0);
	cpu_bus.add(inp,  0x4016, 0x4016, 0);
	cpu_bus.add(inp,  0x4017, 0x4017, 1);
	cpu_bus.add(apu,  0x4017, 0x4017, 0);
	cpu_bus.add(cart, 0x4020, 0xffff, 0);

	ppu_bus.add(cart, 0x0000, 0x3eff, 0);
	ppu_bus.add(pal,  0x3f00, 0x3fff, 0);
}


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void Machine::reset()
{
	apu.reset();
	cart.reset();
	cpu.reset();
	dma.reset();
	inp.reset();
	ppu.reset();

	cpu.nmi(ppu.is_nmi());
	cpu.irq(cart.is_irq());
	cpu.rdy(dma.is_rdy());
}

// Signal: Frame
void Machine::frame()
{
	for (uint16_t i = 0; i != frame_length; ++i)
	{
		ppu.clock();
		ppu.clock();
		ppu.clock();

		dma.clock();
		cpu.clock();
		apu.clock();
		cart.clock();

		cpu.nmi(ppu.is_nmi());
		cpu.irq(cart.is_irq());
		cpu.rdy(dma.is_rdy());
	}
}
