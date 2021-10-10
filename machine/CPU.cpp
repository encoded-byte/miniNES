#include <stdexcept>
#include "machine/CPU.h"


const CPU::Opcode CPU::op_table[256]
{
	// 0x00 - 0x0f
	{ &CPU::BRK, &CPU::IMP, 7, 0, 0 }, { &CPU::ORA, &CPU::IDX, 6, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::NOP, &CPU::ZPG, 3, 0, 0 }, { &CPU::ORA, &CPU::ZPG, 3, 0, 0 }, { &CPU::ASL, &CPU::ZPG, 5, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::PHP, &CPU::IMP, 3, 0, 0 }, { &CPU::ORA, &CPU::IMM, 2, 0, 0 }, { &CPU::ASL, &CPU::ACC, 2, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::NOP, &CPU::ABS, 4, 0, 0 }, { &CPU::ORA, &CPU::ABS, 4, 0, 0 }, { &CPU::ASL, &CPU::ABS, 6, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	// 0x10 - 0x1f
	{ &CPU::BPL, &CPU::REL, 2, 1, 1 }, { &CPU::ORA, &CPU::IDY, 5, 1, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::NOP, &CPU::ZPX, 4, 0, 0 }, { &CPU::ORA, &CPU::ZPX, 4, 0, 0 }, { &CPU::ASL, &CPU::ZPX, 6, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::CLC, &CPU::IMP, 2, 0, 0 }, { &CPU::ORA, &CPU::ABY, 4, 1, 0 }, { &CPU::NOP, &CPU::IMP, 2, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::NOP, &CPU::ABX, 4, 1, 0 }, { &CPU::ORA, &CPU::ABX, 4, 1, 0 }, { &CPU::ASL, &CPU::ABX, 7, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	// 0x20 - 0x2f
	{ &CPU::JSR, &CPU::ABS, 6, 0, 0 }, { &CPU::AND, &CPU::IDX, 6, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::BIT, &CPU::ZPG, 3, 0, 0 }, { &CPU::AND, &CPU::ZPG, 3, 0, 0 }, { &CPU::ROL, &CPU::ZPG, 5, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::PLP, &CPU::IMP, 4, 0, 0 }, { &CPU::AND, &CPU::IMM, 2, 0, 0 }, { &CPU::ROL, &CPU::ACC, 2, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::BIT, &CPU::ABS, 4, 0, 0 }, { &CPU::AND, &CPU::ABS, 4, 0, 0 }, { &CPU::ROL, &CPU::ABS, 6, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	// 0x30 - 0x3f
	{ &CPU::BMI, &CPU::REL, 2, 1, 1 }, { &CPU::AND, &CPU::IDY, 5, 1, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::NOP, &CPU::ZPX, 4, 0, 0 }, { &CPU::AND, &CPU::ZPX, 4, 0, 0 }, { &CPU::ROL, &CPU::ZPX, 6, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::SEC, &CPU::IMP, 2, 0, 0 }, { &CPU::AND, &CPU::ABY, 4, 1, 0 }, { &CPU::NOP, &CPU::IMP, 2, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::NOP, &CPU::ABX, 4, 1, 0 }, { &CPU::AND, &CPU::ABX, 4, 1, 0 }, { &CPU::ROL, &CPU::ABX, 7, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	// 0x40 - 0x4f
	{ &CPU::RTI, &CPU::IMP, 6, 0, 0 }, { &CPU::EOR, &CPU::IDX, 6, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::NOP, &CPU::ZPG, 3, 0, 0 }, { &CPU::EOR, &CPU::ZPG, 3, 0, 0 }, { &CPU::LSR, &CPU::ZPG, 5, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::PHA, &CPU::IMP, 3, 0, 0 }, { &CPU::EOR, &CPU::IMM, 2, 0, 0 }, { &CPU::LSR, &CPU::ACC, 2, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::JMP, &CPU::ABS, 3, 0, 0 }, { &CPU::EOR, &CPU::ABS, 4, 0, 0 }, { &CPU::LSR, &CPU::ABS, 6, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	// 0x50 - 0x5f
	{ &CPU::BVC, &CPU::REL, 2, 1, 1 }, { &CPU::EOR, &CPU::IDY, 5, 1, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::NOP, &CPU::ZPX, 4, 0, 0 }, { &CPU::EOR, &CPU::ZPX, 4, 0, 0 }, { &CPU::LSR, &CPU::ZPX, 6, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::CLI, &CPU::IMP, 2, 0, 0 }, { &CPU::EOR, &CPU::ABY, 4, 1, 0 }, { &CPU::NOP, &CPU::IMP, 2, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::NOP, &CPU::ABX, 4, 1, 0 }, { &CPU::EOR, &CPU::ABX, 4, 1, 0 }, { &CPU::LSR, &CPU::ABX, 7, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	// 0x60 - 0x6f
	{ &CPU::RTS, &CPU::IMP, 6, 0, 0 }, { &CPU::ADC, &CPU::IDX, 6, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::NOP, &CPU::ZPG, 3, 0, 0 }, { &CPU::ADC, &CPU::ZPG, 3, 0, 0 }, { &CPU::ROR, &CPU::ZPG, 5, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::PLA, &CPU::IMP, 4, 0, 0 }, { &CPU::ADC, &CPU::IMM, 2, 0, 0 }, { &CPU::ROR, &CPU::ACC, 2, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::JMP, &CPU::IND, 5, 0, 0 }, { &CPU::ADC, &CPU::ABS, 4, 0, 0 }, { &CPU::ROR, &CPU::ABS, 6, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	// 0x70 - 0x7f
	{ &CPU::BVS, &CPU::REL, 2, 1, 1 }, { &CPU::ADC, &CPU::IDY, 5, 1, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::NOP, &CPU::ZPX, 4, 0, 0 }, { &CPU::ADC, &CPU::ZPX, 4, 0, 0 }, { &CPU::ROR, &CPU::ZPX, 6, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::SEI, &CPU::IMP, 2, 0, 0 }, { &CPU::ADC, &CPU::ABY, 4, 1, 0 }, { &CPU::NOP, &CPU::IMP, 2, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::NOP, &CPU::ABX, 4, 1, 0 }, { &CPU::ADC, &CPU::ABX, 4, 1, 0 }, { &CPU::ROR, &CPU::ABX, 7, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	// 0x80 - 0x8f
	{ &CPU::NOP, &CPU::IMM, 2, 0, 0 }, { &CPU::STA, &CPU::IDX, 6, 0, 0 }, { &CPU::NOP, &CPU::IMM, 2, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::STY, &CPU::ZPG, 3, 0, 0 }, { &CPU::STA, &CPU::ZPG, 3, 0, 0 }, { &CPU::STX, &CPU::ZPG, 3, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::DEY, &CPU::IMP, 2, 0, 0 }, { &CPU::NOP, &CPU::IMM, 2, 0, 0 }, { &CPU::TXA, &CPU::IMP, 2, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::STY, &CPU::ABS, 4, 0, 0 }, { &CPU::STA, &CPU::ABS, 4, 0, 0 }, { &CPU::STX, &CPU::ABS, 4, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	// 0x90 - 0x9f
	{ &CPU::BCC, &CPU::REL, 2, 1, 1 }, { &CPU::STA, &CPU::IDY, 6, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::STY, &CPU::ZPX, 4, 0, 0 }, { &CPU::STA, &CPU::ZPX, 4, 0, 0 }, { &CPU::STX, &CPU::ZPY, 4, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::TYA, &CPU::IMP, 2, 0, 0 }, { &CPU::STA, &CPU::ABY, 5, 0, 0 }, { &CPU::TXS, &CPU::IMP, 2, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::KIL, &CPU::IMP, 0, 0, 0 }, { &CPU::STA, &CPU::ABX, 5, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	// 0xa0 - 0xaf
	{ &CPU::LDY, &CPU::IMM, 2, 0, 0 }, { &CPU::LDA, &CPU::IDX, 6, 0, 0 }, { &CPU::LDX, &CPU::IMM, 2, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::LDY, &CPU::ZPG, 3, 0, 0 }, { &CPU::LDA, &CPU::ZPG, 3, 0, 0 }, { &CPU::LDX, &CPU::ZPG, 3, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::TAY, &CPU::IMP, 2, 0, 0 }, { &CPU::LDA, &CPU::IMM, 2, 0, 0 }, { &CPU::TAX, &CPU::IMP, 2, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::LDY, &CPU::ABS, 4, 0, 0 }, { &CPU::LDA, &CPU::ABS, 4, 0, 0 }, { &CPU::LDX, &CPU::ABS, 4, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	// 0xb0 - 0xbf
	{ &CPU::BCS, &CPU::REL, 2, 1, 1 }, { &CPU::LDA, &CPU::IDY, 5, 1, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::LDY, &CPU::ZPX, 4, 0, 0 }, { &CPU::LDA, &CPU::ZPX, 4, 0, 0 }, { &CPU::LDX, &CPU::ZPY, 4, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::CLV, &CPU::IMP, 2, 0, 0 }, { &CPU::LDA, &CPU::ABY, 4, 1, 0 }, { &CPU::TSX, &CPU::IMP, 2, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::LDY, &CPU::ABX, 4, 1, 0 }, { &CPU::LDA, &CPU::ABX, 4, 1, 0 }, { &CPU::LDX, &CPU::ABY, 4, 1, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	// 0xc0 - 0xcf
	{ &CPU::CPY, &CPU::IMM, 2, 0, 0 }, { &CPU::CMP, &CPU::IDX, 6, 0, 0 }, { &CPU::NOP, &CPU::IMM, 2, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::CPY, &CPU::ZPG, 3, 0, 0 }, { &CPU::CMP, &CPU::ZPG, 3, 0, 0 }, { &CPU::DEC, &CPU::ZPG, 5, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::INY, &CPU::IMP, 2, 0, 0 }, { &CPU::CMP, &CPU::IMM, 2, 0, 0 }, { &CPU::DEX, &CPU::IMP, 2, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::CPY, &CPU::ABS, 4, 0, 0 }, { &CPU::CMP, &CPU::ABS, 4, 0, 0 }, { &CPU::DEC, &CPU::ABS, 6, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	// 0xd0 - 0xdf
	{ &CPU::BNE, &CPU::REL, 2, 1, 1 }, { &CPU::CMP, &CPU::IDY, 5, 1, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::NOP, &CPU::ZPX, 4, 0, 0 }, { &CPU::CMP, &CPU::ZPX, 4, 0, 0 }, { &CPU::DEC, &CPU::ZPX, 6, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::CLD, &CPU::IMP, 2, 0, 0 }, { &CPU::CMP, &CPU::ABY, 4, 1, 0 }, { &CPU::NOP, &CPU::IMP, 2, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::NOP, &CPU::ABX, 4, 1, 0 }, { &CPU::CMP, &CPU::ABX, 4, 1, 0 }, { &CPU::DEC, &CPU::ABX, 7, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	// 0xe0 - 0xef
	{ &CPU::CPX, &CPU::IMM, 2, 0, 0 }, { &CPU::SBC, &CPU::IDX, 6, 0, 0 }, { &CPU::NOP, &CPU::IMM, 2, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::CPX, &CPU::ZPG, 3, 0, 0 }, { &CPU::SBC, &CPU::ZPG, 3, 0, 0 }, { &CPU::INC, &CPU::ZPG, 5, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::INX, &CPU::IMP, 2, 0, 0 }, { &CPU::SBC, &CPU::IMM, 2, 0, 0 }, { &CPU::NOP, &CPU::IMP, 2, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::CPX, &CPU::ABS, 4, 0, 0 }, { &CPU::SBC, &CPU::ABS, 4, 0, 0 }, { &CPU::INC, &CPU::ABS, 6, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	// 0xf0 - 0xff
	{ &CPU::BEQ, &CPU::REL, 2, 1, 1 }, { &CPU::SBC, &CPU::IDY, 5, 1, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::NOP, &CPU::ZPX, 4, 0, 0 }, { &CPU::SBC, &CPU::ZPX, 4, 0, 0 }, { &CPU::INC, &CPU::ZPX, 6, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::SED, &CPU::IMP, 2, 0, 0 }, { &CPU::SBC, &CPU::ABY, 4, 1, 0 }, { &CPU::NOP, &CPU::IMP, 2, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
	{ &CPU::NOP, &CPU::ABX, 4, 1, 0 }, { &CPU::SBC, &CPU::ABX, 4, 1, 0 }, { &CPU::INC, &CPU::ABX, 7, 0, 0 }, { &CPU::KIL, &CPU::IMP, 0, 0, 0 },
};


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void CPU::reset()
{
	uint16_t lo = bus.read(reset_addr);
	uint16_t hi = bus.read(reset_addr + 1) << 8;
	pc = hi | lo;
	status = 0x04;
	ac = 0;
	x = 0;
	y = 0;
	sp = 0xfd;
	nmi_edge = 0;
	nmi_pending = 0;
	irq_pending = 0;
	cycles = 7;
}

// Signal: Clock
void CPU::clock()
{
	if (rdy_line && --cycles == 0)
	{
		if (nmi_pending)
			exec_nmi();
		else if (irq_pending)
			exec_irq();
		else
			exec_opcode();

		nmi_pending = nmi_edge;
		irq_pending = irq_line && !status.i;
		nmi_edge = 0;
	}
}

// Signal: Execute Opcode
void CPU::exec_opcode()
{
	page_crossed = 0;
	branch = 0;
	acc_mode = 0;
	uint8_t data = bus.read(pc++);
	auto &opcode = op_table[data];

	(this->*opcode.mode)();
	(this->*opcode.instruction)();

	cycles = opcode.cycles;
	if (opcode.branch)
		cycles += branch + (branch && page_crossed);
	else if (opcode.page_crossed)
		cycles += page_crossed;
}

// Signal: Interrupt Request
void CPU::exec_irq()
{
	bus.write(stack | sp--, pc >> 8);
	bus.write(stack | sp--, pc & 0xff);
	bus.write(stack | sp--, status);
	uint16_t lo = bus.read(irq_addr);
	uint16_t hi = bus.read(irq_addr + 1) << 8;
	pc = hi | lo;
	status.i = 1;
	cycles = 7;
}

// Signal: Non Maskable Interrupt
void CPU::exec_nmi()
{
	bus.write(stack | sp--, pc >> 8);
	bus.write(stack | sp--, pc & 0xff);
	bus.write(stack | sp--, status);
	uint16_t lo = bus.read(nmi_addr);
	uint16_t hi = bus.read(nmi_addr + 1) << 8;
	pc = hi | lo;
	status.i = 1;
	cycles = 7;
}


//////////////////////////////////////////////////////////////////////////////
//
//                             ADDRESSING MODES
//
//////////////////////////////////////////////////////////////////////////////


// Addressing Mode: Implied
void CPU::IMP()
{
}

// Addressing Mode: Accumulator
void CPU::ACC()
{
	acc_mode = 1;
}

// Addressing Mode: Immediate
void CPU::IMM()
{
	address = pc++;
}

// Addresing Mode: Zero Page
void CPU::ZPG()
{
	address = bus.read(pc++);
}

// Addresing Mode: Zero Page, X
void CPU::ZPX()
{
	address = bus.read(pc++);
	address = (address + x) & 0xff;
}

// Addresing Mode: Zero Page, Y
void CPU::ZPY()
{
	address = bus.read(pc++);
	address = (address + y) & 0xff;
}

// Addresing Mode: Relative
void CPU::REL()
{
	address = bus.read(pc++);
	if (address & 0x80)
		address |= 0xff00;
	address += pc;
	page_crossed = (pc & 0xff00) != (address & 0xff00);
}

// Addresing Mode: Absolute
void CPU::ABS()
{
	uint16_t lo = bus.read(pc++);
	uint16_t hi = bus.read(pc++) << 8;
	address = hi | lo;
}

// Addresing Mode: Absolute, X
void CPU::ABX()
{
	uint16_t lo = bus.read(pc++);
	uint16_t hi = bus.read(pc++) << 8;
	address = (hi | lo) + x;
	page_crossed = (hi & 0xff00) != (address & 0xff00);
}

// Addresing Mode: Absolute, Y
void CPU::ABY()
{
	uint16_t lo = bus.read(pc++);
	uint16_t hi = bus.read(pc++) << 8;
	address = (hi | lo) + y;
	page_crossed = (hi & 0xff00) != (address & 0xff00);
}

// Addresing Mode: Indirect
void CPU::IND()
{
	uint16_t lo = bus.read(pc++);
	uint16_t hi = bus.read(pc++) << 8;
	address = hi | lo;
	lo = bus.read(address);
	hi = bus.read((address & 0xff00) | ((address + 1) & 0x00ff)) << 8;
	address = hi | lo;
}

// Addresing Mode: Indexed Indirect
void CPU::IDX()
{
	address = bus.read(pc++);
	address = (address + x) & 0xff;
	uint16_t lo = bus.read(address);
	uint16_t hi = bus.read((address + 1) & 0xff) << 8;
	address = hi | lo;
}

// Addressing Mode: Indirect Indexed
void CPU::IDY()
{
	address = bus.read(pc++);
	uint16_t lo = bus.read(address);
	uint16_t hi = bus.read((address + 1) & 0xff) << 8;
	address = (hi | lo) + y;
	page_crossed = (hi & 0xff00) != (address & 0xff00);
}


//////////////////////////////////////////////////////////////////////////////
//
//                               INSTRUCTIONS
//
//////////////////////////////////////////////////////////////////////////////


// Instruction: NOP (No Operation)
void CPU::NOP()
{
}

// Instruction: CLC (Clear Carry)
void CPU::CLC()
{
	status.c = 0;
}

// Instruction: CLD (Clear Decimal)
void CPU::CLD()
{
	status.d = 0;
}

// Instruction: CLI (Clear Interrupt)
void CPU::CLI()
{
	status.i = 0;
}

// Instruction: CLV (Clear Overflow)
void CPU::CLV()
{
	status.v = 0;
}

// Instruction: SEC (Set Carry)
void CPU::SEC()
{
	status.c = 1;
}

// Instruction: SED (Set Decimal)
void CPU::SED()
{
	status.d = 1;
}

// Instruction: SEI (Set Interrupt)
void CPU::SEI()
{
	status.i = 1;
}

// Instruction: LDA (Load Accumulator)
void CPU::LDA()
{
	ac = bus.read(address);
	status.z = ac == 0;
	status.n = ac & 0x80;
}

// Instruction: LDX (Load X)
void CPU::LDX()
{
	x = bus.read(address);
	status.z = x == 0;
	status.n = x & 0x80;
}

// Instruction: LDY (Load Y)
void CPU::LDY()
{
	y = bus.read(address);
	status.z = y == 0;
	status.n = y & 0x80;
}

// Instruction: STA (Store Accumulator)
void CPU::STA()
{
	bus.write(address, ac);
}

// Instruction: STX (Store X)
void CPU::STX()
{
	bus.write(address, x);
}

// Instruction: STY (Store Y)
void CPU::STY()
{
	bus.write(address, y);
}

// Instruction: TAX (Transfer Accumulator to X)
void CPU::TAX()
{
	x = ac;
	status.z = x == 0;
	status.n = x & 0x80;
}

// Instruction: TAY (Transfer Accumulator to Y)
void CPU::TAY()
{
	y = ac;
	status.z = y == 0;
	status.n = y & 0x80;
}

// Instruction: TXA (Transfer X to Accumulator)
void CPU::TXA()
{
	ac = x;
	status.z = ac == 0;
	status.n = ac & 0x80;
}

// Instruction: TYA (Transfer Y to Accumulator)
void CPU::TYA()
{
	ac = y;
	status.z = ac == 0;
	status.n = ac & 0x80;
}

// Instruction: TSX (Transfer Stack Pointer to X)
void CPU::TSX()
{
	x = sp;
	status.z = x == 0;
	status.n = x & 0x80;
}

// Instruction: TXS (Transfer X to Stack Pointer)
void CPU::TXS()
{
	sp = x;
}

// Instruction: BIT (Bit Test)
void CPU::BIT()
{
	uint8_t data = bus.read(address);
	status.z = (data & ac) == 0;
	status.v = data & 0x40;
	status.n = data & 0x80;
}

// Instruction: CMP (Compare Accumulator)
void CPU::CMP()
{
	uint8_t data = bus.read(address);
	status.c = ac >= data;
	data = ac - data;
	status.z = data == 0;
	status.n = data & 0x80;
}

// Instruction: CPX (Compare X)
void CPU::CPX()
{
	uint8_t data = bus.read(address);
	status.c = x >= data;
	data = x - data;
	status.z = data == 0;
	status.n = data & 0x80;
}

// Instruction: CPY (Compare Y)
void CPU::CPY()
{
	uint8_t data = bus.read(address);
	status.c = y >= data;
	data = y - data;
	status.z = data == 0;
	status.n = data & 0x80;
}

// Instuction: AND (AND with Accumulator)
void CPU::AND()
{
	ac &= bus.read(address);
	status.z = ac == 0;
	status.n = ac & 0x80;
}

// Instruction: EOR (Exclusive OR with Accumulator)
void CPU::EOR()
{
	ac ^= bus.read(address);
	status.z = ac == 0;
	status.n = ac & 0x80;
}

// Instruction: ORA (OR with Accumulator)
void CPU::ORA()
{
	ac |= bus.read(address);
	status.z = ac == 0;
	status.n = ac & 0x80;
}

// Instruction: INC (Increment Memory)
void CPU::INC()
{
	uint8_t data = bus.read(address) + 1;
	bus.write(address, data);
	status.z = data == 0;
	status.n = data & 0x80;
}

// Instruction: INX (Increment X)
void CPU::INX()
{
	++x;
	status.z = x == 0;
	status.n = x & 0x80;
}

// Instruction: INY (Increment Y)
void CPU::INY()
{
	++y;
	status.z = y == 0;
	status.n = y & 0x80;
}

// Instruction: DEC (Decrement Memory)
void CPU::DEC()
{
	uint8_t data = bus.read(address) - 1;
	bus.write(address, data);
	status.z = data == 0;
	status.n = data & 0x80;
}

// Instruction: DEX (Decrement X)
void CPU::DEX()
{
	--x;
	status.z = x == 0;
	status.n = x & 0x80;
}

// Instruction: DEY (Decrement Y)
void CPU::DEY()
{
	--y;
	status.z = y == 0;
	status.n = y & 0x80;
}

// Instruction: ADC (Add with Carry)
void CPU::ADC()
{
	uint8_t data = bus.read(address);
	uint16_t result = ac + data + status.c;
	status.v = ~(data ^ ac) & (result ^ ac) & 0x80;
	ac = result;
	status.c = result & 0xff00;
	status.z = ac == 0;
	status.n = ac & 0x80;
}

// Instruction: SBC (Subtract with Carry)
void CPU::SBC()
{
	uint8_t data = bus.read(address) ^ 0xff;
	uint16_t result = ac + data + status.c;
	status.v = (data ^ result) & (result ^ ac) & 0x80;
	ac = result;
	status.c = result & 0xff00;
	status.z = ac == 0;
	status.n = ac & 0x80;
}

// Instruction: ASL (Arithmetic Shift Left)
void CPU::ASL()
{
	if (acc_mode)
	{
		status.c = ac & 0x80;
		ac <<= 1;
		status.z = ac == 0;
		status.n = ac & 0x80;
	}
	else
	{
		uint8_t data = bus.read(address);
		status.c = data & 0x80;
		data <<= 1;
		bus.write(address, data);
		status.z = data == 0;
		status.n = data & 0x80;
	}
}

// Instruction: LSR (Logical Shift Right)
void CPU::LSR()
{
	if (acc_mode)
	{
		status.c = ac & 0x01;
		ac >>= 1;
		status.z = ac == 0;
		status.n = ac & 0x80;
	}
	else
	{
		uint8_t data = bus.read(address);
		status.c = data & 0x01;
		data >>= 1;
		bus.write(address, data);
		status.z = data == 0;
		status.n = data & 0x80;
	}
}

// Instruction: ROL (Rotate Left)
void CPU::ROL()
{
	if (acc_mode)
	{
		bool carry = ac & 0x80;
		ac = (ac << 1) | (status.c ? 1 : 0);
		status.c = carry;
		status.z = ac == 0;
		status.n = ac & 0x80;
	}
	else
	{
		uint8_t data = bus.read(address);
		bool carry = data & 0x80;
		data = (data << 1) | (status.c ? 1 : 0);
		bus.write(address, data);
		status.c = carry;
		status.z = data == 0;
		status.n = data & 0x80;
	}
}

// Instruction: ROR (Rotate Right)
void CPU::ROR()
{
	if (acc_mode)
	{
		bool carry = ac & 0x01;
		ac = (ac >> 1) | (status.c << 7);
		status.c = carry;
		status.z = ac == 0;
		status.n = ac & 0x80;
	}
	else
	{
		uint8_t data = bus.read(address);
		bool carry = data & 0x01;
		data = (data >> 1) | (status.c << 7);
		bus.write(address, data);
		status.c = carry;
		status.z = data == 0;
		status.n = data & 0x80;
	}
}

// Instruction: BCC (Branch if Carry Clear)
void CPU::BCC()
{
	branch = !status.c;
	if (branch)
		pc = address;
}

// Instruction: BCS (Branch if Carry Set)
void CPU::BCS()
{
	branch = status.c;
	if (branch)
		pc = address;
}

// Instruction: BNE (Branch if Not Equal)
void CPU::BNE()
{
	branch = !status.z;
	if (branch)
		pc = address;
}

// Instruction: BEQ (Branch if Equal)
void CPU::BEQ()
{
	branch = status.z;
	if (branch)
		pc = address;
}

// Instruction: BPL (Branch if Plus)
void CPU::BPL()
{
	branch = !status.n;
	if (branch)
		pc = address;
}

// Instruction: BMI (Branch if Minus)
void CPU::BMI()
{
	branch = status.n;
	if (branch)
		pc = address;
}

// Instruction: BVC (Branch if Overflow Clear)
void CPU::BVC()
{
	branch = !status.v;
	if (branch)
		pc = address;
}

// Instruction: BVS (Branch if Overflow Set)
void CPU::BVS()
{
	branch = status.v;
	if (branch)
		pc = address;
}

// Instruction: JMP (Jump)
void CPU::JMP()
{
	pc = address;
}

// Instruction: PHA (Push Accumulator)
void CPU::PHA()
{
	bus.write(stack | sp--, ac);
}

// Instruction: PHP (Push Processor Status)
void CPU::PHP()
{
	bus.write(stack | sp--, status | 0x10);
}

// Instruction: PLA (Pull Accumulator)
void CPU::PLA()
{
	ac = bus.read(stack | ++sp);
	status.z = ac == 0;
	status.n = ac & 0x80;
}

// Instruction: PLP (Pull Processor Status)
void CPU::PLP()
{
	status = bus.read(stack | ++sp);
}

// Instruction: JSR (Jump To Subroutine)
void CPU::JSR()
{
	--pc;
	bus.write(stack | sp--, pc >> 8);
	bus.write(stack | sp--, pc & 0xff);
	pc = address;
}

// Instruction: RTS (Return from Subroutine)
void CPU::RTS()
{
	uint16_t lo = bus.read(stack | ++sp);
	uint16_t hi = bus.read(stack | ++sp) << 8;
	pc = (hi | lo) + 1;
}

// Instruction: BRK (Break)
void CPU::BRK()
{
	++pc;
	bus.write(stack | sp--, pc >> 8);
	bus.write(stack | sp--, pc & 0xff);
	bus.write(stack | sp--, status | 0x10);
	uint16_t lo = bus.read(irq_addr);
	uint16_t hi = bus.read(irq_addr + 1) << 8;
	pc = hi | lo;
	status.i = 1;
}

// Instruction: RTI (Return from Interrupt)
void CPU::RTI()
{
	status = bus.read(stack | ++sp);
	uint16_t lo = bus.read(stack | ++sp);
	uint16_t hi = bus.read(stack | ++sp) << 8;
	pc = hi | lo;
}

// Instruction: KIL (Illegal Opcode)
void CPU::KIL()
{
	throw std::runtime_error("CPU: Unsupported opcode");
}
