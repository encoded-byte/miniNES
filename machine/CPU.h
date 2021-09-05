#pragma once

#include "devices/Bus.h"


class CPU
{
private:
	Bus &bus;
	const uint16_t stack      {0x0100}; // Stack address
	const uint16_t nmi_addr   {0xfffa}; // NMI address
	const uint16_t reset_addr {0xfffc}; // Reset address
	const uint16_t irq_addr   {0xfffe}; // IRQ address

	// Opcodes
	struct Opcode
	{
		using funct_ptr = void (CPU::*)();
		funct_ptr instruction;
		funct_ptr mode;
		uint8_t cycles;
		bool page_crossed;
		bool branch;
	};
	static const Opcode op_table[256];

	// Registers
	struct Status
	{
		void operator=(const uint8_t &u8)
			{ n = u8 & 128; v = u8 & 64; d = u8 & 8; i = u8 & 4; z = u8 & 2; c = u8 & 1; }
		operator uint8_t() const
			{ return n << 7 | v << 6 | 1 << 5 | d << 3 | i << 2 | z << 1 | c << 0; }

		bool n; // Negative flag
		bool v; // Overflow flag
		bool d; // Decimal mode flag
		bool i; // Interrupt disable
		bool z; // Zero flag
		bool c; // Carry flag
	};
	Status status; // Status register
	uint16_t pc;   // Program counter
	uint8_t ac;    // Accumulator
	uint8_t x;     // Index register X
	uint8_t y;     // Index register Y
	uint8_t sp;    // Stack pointer

	// Status
	bool page_crossed; // Set if page crossed
	bool branch;       // Set if branch taken
	bool acc_mode;     // Set if accumulator mode
	bool nmi_edge;     // Set if NMI edge
	bool nmi_pending;  // Set if NMI pending
	bool irq_pending;  // Set if IRQ pending
	bool nmi_line;     // NMI line
	bool irq_line;     // IRQ line
	bool rdy_line;     // RDY line
	uint16_t address;  // Target address
	uint8_t cycles;    // Cycles left to complete opcode

	// Addressing Modes
	void IMP(); void ACC(); void IMM(); void ZPG(); void ZPX(); void ZPY(); void REL(); void ABS();
	void ABX(); void ABY(); void IND(); void IDX(); void IDY();

	// Instructions
	void ADC(); void AND(); void ASL(); void BCC(); void BCS(); void BEQ(); void BIT(); void BMI();
	void BNE(); void BPL(); void BRK(); void BVC(); void BVS(); void CLC(); void CLD(); void CLI();
	void CLV(); void CMP(); void CPX(); void CPY(); void DEC(); void DEX(); void DEY(); void EOR();
	void INC(); void INX(); void INY(); void JMP(); void JSR(); void LDA(); void LDX(); void LDY();
	void LSR(); void NOP(); void ORA(); void PHA(); void PHP(); void PLA(); void PLP(); void ROL();
	void ROR(); void RTI(); void RTS(); void SBC(); void SEC(); void SED(); void SEI(); void STA();
	void STX(); void STY(); void TAX(); void TAY(); void TSX(); void TXA(); void TXS(); void TYA();
	void KIL();

	// Signals
	void exec_opcode();
	void exec_irq();
	void exec_nmi();

public:
	CPU(Bus &bus) : bus(bus) {}

	// Signals
	void reset();
	void clock();
	void nmi(bool line) { if (line && !nmi_line) nmi_edge = 1; nmi_line = line; }
	void irq(bool line) { irq_line = line; }
	void rdy(bool line) { rdy_line = line; }
};
