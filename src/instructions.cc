//
// 6502 instruction/opcode implementations
//
// Copyright (C) 2023 Walt Drummond
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.

#include "6502.h"

//////////
// Helper functions

// Set PC to @address if @condition is true
void MOS6502::doBranch(bool condition, Word address, Cycles_t &expectedCyclesToUse) {
	if (condition) {
		Cycles++;	// Branch taken
		expectedCyclesToUse++;

		if ((PC >> 8) != (address >> 8)) { // Crossed page boundary
			Cycles += 2;
			expectedCyclesToUse += 2;
		}

		PC = address;
	}
}

// BCD addition and subtraction functions.
// See:
// https://www.electrical4u.com/bcd-or-binary-coded-decimal-bcd-conversion-addition-subtraction/
void MOS6502::bcdADC(Byte operand) {
	Byte addend, carry, a_low;
	int answer;

	addend = A;
	carry = Flags.C;

	// Low nibble first
	a_low = (Byte) ((addend & 0x0f) + (operand & 0x0f) + carry);
	if (a_low >= 0x0a) 
		a_low = ((a_low + 0x06) & 0x0f) + 0x10;

	answer = (addend & 0xf0) + (operand & 0xf0) + a_low;
	if (answer >= 0xa0) 
		answer += 0x60;
	
	A = (Word) (answer & 0xff);
	
	setFlagNByValue(A);
	setFlagZByValue(A);
	Flags.C = (answer >= 0x100);
	Flags.V = (answer < -128) || (answer > 127);
}

void MOS6502::bcdSBC(Byte subtrahend) {
	SByte op_l;
	int operand;
	Byte carry;

	operand = A;
	carry = (Flags.C == 0);

	// Low nibble first
	op_l = (SByte) ((operand & 0x0f) - (subtrahend & 0x0f) - carry);
	if (op_l < 0) 
		op_l = ((op_l - 0x06) & 0x0f) - 0x10;
					
	operand = (operand & 0xf0) - (subtrahend & 0xf0);
	operand += op_l;
	if (operand < 0) 
		operand -= 0x60;

	A = (Byte) operand & 0xff;

	setFlagZByValue(A);
	Flags.C = (operand >= 0);
}

// A = A + operand + Flags.C
void MOS6502::doADC(Byte operand) {
	Word result;
	bool same_sign;

	same_sign = isNegative(A) == isNegative(operand);
	result = A + operand + Flags.C;
	A = result & 0xff;
	setFlagZByValue(A);
	setFlagNByValue(A);
	Flags.C = result > 0xff;
	Flags.V = same_sign && (isNegative(A) != isNegative(operand));
}

////
// CPU Instructions

// ADC
void MOS6502::ins_adc(Byte opcode, Cycles_t &expectedCyclesToUse) {
	Byte operand = getData(opcode, expectedCyclesToUse);
	
	if (Flags.D) {
		bcdADC(operand);
		return;
	}

	doADC(operand);
}

// AND
void MOS6502::ins_and(Byte opcode, Cycles_t &expectedCyclesToUse) {
	Byte data;

	data = getData(opcode, expectedCyclesToUse);
	A &= data;
	setFlagZByValue(A);
	setFlagNByValue(A);
}

// ASL
void MOS6502::ins_asl(Byte opcode, Cycles_t &expectedCyclesToUse) {
	Word address;
	Byte data;
	bool accumulator = _instructions.at(opcode).addrmode == AddressingMode::Accumulator;

	if (accumulator) {
		data = A;
	} else {
		address = getAddress(opcode, expectedCyclesToUse);
		data = readByte(address);
	}

	Flags.C = isNegative(data);
	data = data << 1;
	setFlagNByValue(data);
	setFlagZByValue(data);

	if (accumulator) {
		A = data;
	} else {
		writeByte(address, data);
	}
	
	Cycles++;
	if (_instructions.at(opcode).addrmode == AddressingMode::AbsoluteX)
		Cycles++;	
}

// BCC
void MOS6502::ins_bcc(Byte opcode, Cycles_t &expectedCyclesToUse) {
	Word address;

	address = getAddress(opcode, expectedCyclesToUse);
	doBranch(!Flags.C, address, expectedCyclesToUse);
}

// BCS
void MOS6502::ins_bcs(Byte opcode, Cycles_t &expectedCyclesToUse) {
	Word address;

	address = getAddress(opcode, expectedCyclesToUse);
	doBranch(Flags.C, address, expectedCyclesToUse);
}

// BEQ
void MOS6502::ins_beq(Byte opcode, Cycles_t &expectedCyclesToUse) {
	Word address;

	address = getAddress(opcode, expectedCyclesToUse);
	doBranch(Flags.Z, address, expectedCyclesToUse);
}

// BIT
void MOS6502::ins_bit(Byte opcode, Cycles_t &expectedCyclesToUse) {
	Byte data;

	data = getData(opcode, expectedCyclesToUse);
	setFlagZByValue(A & data);
	setFlagNByValue(data);
	Flags.V = (data & (1 << 6)) != 0;
}

// BMI
void MOS6502::ins_bmi(Byte opcode, Cycles_t &expectedCyclesToUse) {
	Word address;

	address = getAddress(opcode, expectedCyclesToUse);
	doBranch(Flags.N, address, expectedCyclesToUse);
}

// BNE
void MOS6502::ins_bne(Byte opcode, Cycles_t &expectedCyclesToUse) {
	Word address;

	address = getAddress(opcode, expectedCyclesToUse);
	doBranch(!Flags.Z, address, expectedCyclesToUse);
}

// BPL
void MOS6502::ins_bpl(Byte opcode, Cycles_t &expectedCyclesToUse) {
	Word address;

	address = getAddress(opcode, expectedCyclesToUse);
	doBranch(!Flags.N, address, expectedCyclesToUse);
}

// BRK
void MOS6502::ins_brk([[maybe_unused]] Byte opcode,
		  [[maybe_unused]] Cycles_t &expectedCyclesToUse) {
	// push PC + 1 to the stack. See:
	// https://retrocomputing.stackexchange.com/questions/12291/what-are-uses-of-the-byte-after-brk-instruction-on-6502
	addBacktrace(PC - 1);
	PC++;
	_BRKCount++;
	interrupt();
	Flags.B = 1;
}

// BVC
void MOS6502::ins_bvc(Byte opcode, Cycles_t &expectedCyclesToUse) {
	Word address;

	address = getAddress(opcode, expectedCyclesToUse);
	doBranch(!Flags.V, address, expectedCyclesToUse);
}

// BVS
void MOS6502::ins_bvs(Byte opcode, Cycles_t &expectedCyclesToUse) {
	Word address;

	address = getAddress(opcode, expectedCyclesToUse);
	doBranch(Flags.V, address, expectedCyclesToUse);
}

// CLC
void MOS6502::ins_clc([[maybe_unused]] Byte opcode,
		  [[maybe_unused]] Cycles_t &expectedCyclesToUse) {
	Flags.C = 0;
	Cycles++;		// Single byte instruction
}

// CLD
void MOS6502::ins_cld([[maybe_unused]] Byte opcode,
		  [[maybe_unused]] Cycles_t &expectedCyclesToUse) {
	Flags.D = 0;
	Cycles++;		// Single byte instruction
}

// CLI
void MOS6502::ins_cli([[maybe_unused]] Byte opcode,
		  [[maybe_unused]] Cycles_t &expectedCyclesToUse) {
	Flags.I = 0;
	Cycles++;		// Single byte instruction
}

// CLV
void MOS6502::ins_clv([[maybe_unused]] Byte opcode,
		  [[maybe_unused]] Cycles_t &expectedCyclesToUse) {
	Flags.V = 0;
	Cycles++;		// Single byte instruction
}

// CMP
void MOS6502::ins_cmp(Byte opcode, Cycles_t &expectedCyclesToUse) {
	Byte data = getData(opcode, expectedCyclesToUse);

	Flags.C = A >= data;
	Flags.Z = A == data;

	Byte result = A - data;
	setFlagNByValue(result);
}

// CPX
void MOS6502::ins_cpx(Byte opcode, Cycles_t &expectedCyclesToUse) {
	Byte data = getData(opcode, expectedCyclesToUse);

	Flags.C = X >= data;
	Flags.Z = X == data;

	Byte result = X - data;
	setFlagNByValue(result);
}

// CPY
void MOS6502::ins_cpy(Byte opcode, Cycles_t &expectedCyclesToUse) {
	Byte data = getData(opcode, expectedCyclesToUse);
	Flags.C = Y >= data;
	Flags.Z = Y == data;

	Byte result = Y - data;
	setFlagNByValue(result);
}

// DEC
void MOS6502::ins_dec(Byte opcode, Cycles_t &expectedCyclesToUse) {
	Word address;
	Byte data;

	address = getAddress(opcode, expectedCyclesToUse);
	data = readByte(address);
	data--;
	writeByte(address, data);
	setFlagZByValue(data);
	setFlagNByValue(data);
	Cycles++;
	if (_instructions.at(opcode).addrmode == AddressingMode::AbsoluteX)
		Cycles++;
}

// DEX
void MOS6502::ins_dex([[maybe_unused]] Byte opcode,
		  [[maybe_unused]] Cycles_t &expectedCyclesToUse) {
	X--;
	setFlagNByValue(X);
	setFlagZByValue(X);
	Cycles++;
}

// DEY
void MOS6502::ins_dey([[maybe_unused]] Byte opcode,
		  [[maybe_unused]] Cycles_t &expectedCyclesToUse) {
	Y--;
	setFlagNByValue(Y);
	setFlagZByValue(Y);
	Cycles++;
}

// EOR
void MOS6502::ins_eor(Byte opcode, Cycles_t &expectedCyclesToUse) {
	Byte data;

	data = getData(opcode, expectedCyclesToUse);
	A ^= data;
	setFlagZByValue(A);
	setFlagNByValue(A);
}

// INC
void MOS6502::ins_inc(Byte opcode, Cycles_t &expectedCyclesToUse) {
	Word address;
	Byte data;

	address = getAddress(opcode, expectedCyclesToUse);
	data = readByte(address);
	data++;
	writeByte(address, data);
	setFlagZByValue(data);
	setFlagNByValue(data);
	Cycles++;
	if (_instructions.at(opcode).addrmode == AddressingMode::AbsoluteX)
		Cycles++;
}

// INX
void MOS6502::ins_inx([[maybe_unused]] Byte opcode,
		  [[maybe_unused]] Cycles_t &expectedCyclesToUse) {
	X++;
	setFlagZByValue(X);
	setFlagNByValue(X);
	Cycles++;
}

// INY
void MOS6502::ins_iny([[maybe_unused]] Byte opcode,
		  [[maybe_unused]] Cycles_t &expectedCyclesToUse) {
	Y++;
	setFlagZByValue(Y);
	setFlagNByValue(Y);
	Cycles++;
}

// JMP
void MOS6502::ins_jmp(Byte opcode, [[maybe_unused]] Cycles_t &expectedCyclesToUse) {
	Word address = readWord(PC);
	bool indirect = _instructions.at(opcode).addrmode == AddressingMode::Indirect;
	
	if (indirect) {
		if ((address & 0xff) == 0xff) { // JMP Indirect bug
			Byte lsb = readByte(address);
			Byte msb = readByte(address & 0xff00);
			address = (msb << 8) | lsb;
		} else {
			address = readWord(address);
		}
	} 

	PC = address;
}

// JSR
void MOS6502::ins_jsr([[maybe_unused]] Byte opcode,
		  [[maybe_unused]] Cycles_t &expectedCyclesToUse) {
	Word newPC;
	
	addBacktrace(PC - 1);

	newPC = readWord(PC);
	pushWord(PC + 1);
	PC = newPC;
	
	Cycles++;
}

// LDA
void MOS6502::ins_lda(Byte opcode, Cycles_t &expectedCyclesToUse) {
	A = getData(opcode, expectedCyclesToUse);
	setFlagZByValue(A);
	setFlagNByValue(A);
}

// LDX
void MOS6502::ins_ldx(Byte opcode, Cycles_t &expectedCyclesToUse) {
	X = getData(opcode, expectedCyclesToUse);
	setFlagZByValue(X);
	setFlagNByValue(X);
}

// LDY
void MOS6502::ins_ldy(Byte opcode, Cycles_t &expectedCyclesToUse) {
	Y = getData(opcode, expectedCyclesToUse);
	setFlagZByValue(Y);
	setFlagNByValue(Y);
}

// LSR
void MOS6502::ins_lsr(Byte opcode, Cycles_t &expectedCyclesToUse) {
	Word address;
	Byte data;
	bool accumulator = _instructions.at(opcode).addrmode == AddressingMode::Accumulator;

	if (accumulator)
		data = A;
	else {
		address = getAddress(opcode, expectedCyclesToUse);
		data = readByte(address);
	}

	Flags.C = (data & 1);
	data = data >> 1;
	setFlagZByValue(data);
	setFlagNByValue(data);

	if (accumulator)
		A = data;
	else 
		writeByte(address, data);

	Cycles++;
	if (_instructions.at(opcode).addrmode == AddressingMode::AbsoluteX)
		Cycles++;	
}

// NOP
void MOS6502::ins_nop([[maybe_unused]] Byte opcode,
		  [[maybe_unused]] Cycles_t &expectedCyclesToUse) {
	// NOP, like all single byte instructions, takes
	// two cycles.
	Cycles++;
}

// ORA
void MOS6502::ins_ora(Byte opcode, Cycles_t &expectedCyclesToUse) {
	Byte data;

	data = getData(opcode, expectedCyclesToUse);
	A |= data;
	setFlagNByValue(A);
	setFlagZByValue(A);
}

// PHA
void MOS6502::ins_pha([[maybe_unused]] Byte opcode,
		  [[maybe_unused]] Cycles_t &expectedCyclesToUse) {
	push(A);
	Cycles++;		// Single byte instruction
}

// PHP
void MOS6502::ins_php([[maybe_unused]] Byte opcode,
		  [[maybe_unused]] Cycles_t &expectedCyclesToUse) {
	pushPS();
	Cycles++;		// Single byte instruction
}

// PLA
void MOS6502::ins_pla([[maybe_unused]] Byte opcode,
		  [[maybe_unused]] Cycles_t &expectedCyclesToUse) {
	A = pop();
	setFlagNByValue(A);
	setFlagZByValue(A);
	Cycles += 2;      
}

// PLP
void MOS6502::ins_plp([[maybe_unused]] Byte opcode,
		  [[maybe_unused]] Cycles_t &expectedCyclesToUse) {
	popPS();
	Cycles += 2;
}

// ROL
void MOS6502::ins_rol(Byte opcode, Cycles_t &expectedCyclesToUse) {
	Word address;
	Byte data, carry;
	bool accumulator = _instructions.at(opcode).addrmode == AddressingMode::Accumulator;

	if (accumulator)
		data = A;
	else {
		address = getAddress(opcode, expectedCyclesToUse);
		data = readByte(address);
	}

	carry = Flags.C;
	Flags.C = isNegative(data);

	data = (data << 1) | carry;

	setFlagZByValue(data);
	setFlagNByValue(data);

	if (accumulator)
		A = data;
	else 
		writeByte(address, data);

	Cycles++;
	if (_instructions.at(opcode).addrmode == AddressingMode::AbsoluteX)
		Cycles++;
}

// ROR
void MOS6502::ins_ror(Byte opcode, Cycles_t &expectedCyclesToUse) {
	Word address;
	Byte data, zero;
	bool accumulator = _instructions.at(opcode).addrmode == AddressingMode::Accumulator;

	if (accumulator)
		data = A;
	else {
		address = getAddress(opcode, expectedCyclesToUse);
		data = readByte(address);
	}

	zero = data & 1;
	data = data >> 1;
	if (Flags.C)
		data |= NegativeBit;
	setFlagNByValue(data);
	setFlagZByValue(data);
	Flags.C = (zero == 1);

	if (accumulator)
		A = data;
	else 
		writeByte(address, data);

	Cycles++;
	if (_instructions.at(opcode).addrmode == AddressingMode::AbsoluteX)
		Cycles++;
}

// RTI
void MOS6502::ins_rti([[maybe_unused]] Byte opcode,
		  [[maybe_unused]] Cycles_t &expectedCyclesToUse) {
	removeBacktrace();
	popPS();
	PC = popWord();
	Cycles += 2;
}

// RTS
void MOS6502::ins_rts([[maybe_unused]] Byte opcode,
		  [[maybe_unused]] Cycles_t &expectedCyclesToUse) {
	removeBacktrace();
	
	PC = popWord() + 1;
	Cycles += 3;	       
}

// SBC
void MOS6502::ins_sbc(Byte opcode, Cycles_t &expectedCyclesToUse) {
	Byte operand = getData(opcode, expectedCyclesToUse);

	if (Flags.D) {
		bcdSBC(operand); 
		return;
	}
	
	doADC(~operand);
}

// SEC
void MOS6502::ins_sec([[maybe_unused]] Byte opcode,
		  [[maybe_unused]] Cycles_t &expectedCyclesToUse) {
	Flags.C = 1;
	Cycles++;		// Single byte instruction
}

// SED
void MOS6502::ins_sed([[maybe_unused]] Byte opcode,
		  [[maybe_unused]] Cycles_t &expectedCyclesToUse) {
	Flags.D = 1;
	Cycles++;		// Single byte instruction
}

// SEI
void MOS6502::ins_sei([[maybe_unused]] Byte opcode,
		  [[maybe_unused]] Cycles_t &expectedCyclesToUse) {
	Flags.I = 1;
	Cycles++;		// Single byte instruction
}

// STA
void MOS6502::ins_sta(Byte opcode, Cycles_t &expectedCyclesToUse) {
	Word address = getAddress(opcode, expectedCyclesToUse);
	writeByte(address, A);
}

// STX
void MOS6502::ins_stx(Byte opcode, Cycles_t &expectedCyclesToUse) {
	Word address = getAddress(opcode, expectedCyclesToUse);
	writeByte(address, X);
}

// STY
void MOS6502::ins_sty(Byte opcode, Cycles_t &expectedCyclesToUse) {
	Word address = getAddress(opcode, expectedCyclesToUse);
	writeByte(address, Y);
}

// TAX
void MOS6502::ins_tax([[maybe_unused]] Byte opcode,
		  [[maybe_unused]] Cycles_t &expectedCyclesToUse) {
	X = A;
	setFlagZByValue(X);
	setFlagNByValue(X);
	Cycles++;
}

// TAY
void MOS6502::ins_tay([[maybe_unused]] Byte opcode,
		  [[maybe_unused]] Cycles_t &expectedCyclesToUse) {
	Y = A;
	setFlagZByValue(Y);
	setFlagNByValue(Y);
	Cycles++;
}

// TSX
void MOS6502::ins_tsx([[maybe_unused]] Byte opcode,
		  [[maybe_unused]] Cycles_t &expectedCyclesToUse) {
	X = SP;
	setFlagZByValue(X);
	setFlagNByValue(X);
	Cycles++;
}

// TXA
void MOS6502::ins_txa([[maybe_unused]] Byte opcode,
		  [[maybe_unused]] Cycles_t &expectedCyclesToUse) {
	A = X;
	setFlagZByValue(A);
	setFlagNByValue(A);
	Cycles++;
}

// TXS
void MOS6502::ins_txs([[maybe_unused]] Byte opcode,
		  [[maybe_unused]] Cycles_t &expectedCyclesToUse) {
	SP = X;
	Cycles++;
}

// TYA
void MOS6502::ins_tya([[maybe_unused]] Byte opcode,
		  [[maybe_unused]] Cycles_t &expectedCyclesToUse) {
	A = Y;
	setFlagZByValue(A);
	setFlagNByValue(A);
	Cycles++;
}
