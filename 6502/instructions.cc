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

#include <6502.h>

//////////
// Helper functions

// The shift and rotate instructions (ASL, LSR, ROL, ROR) can operate on A implicitly, or on 
// data in memory.  These helpers make that a bit easier.
void MOS6502::getAorData(Byte& data, const Byte opcode, Word& address) {
	bool accumulator = _instructions.at(opcode).addrmode == AddressingMode::Accumulator;

	if (accumulator)
		data = A;
	else {
		address = getAddress(opcode);
		data = readByte(address);
	}
}

void MOS6502::putAorData(Byte data, const Byte opcode, Word address) {
	bool accumulator = _instructions.at(opcode).addrmode == AddressingMode::Accumulator;

	if (accumulator)
		A = data;
	else 
		writeByte(address, data);
}

// Set PC to @address if @condition is true
void MOS6502::doBranch(bool condition, const Byte opcode) {
	Word address = getAddress(opcode);

	if (condition) {
		_cycles++;	// Branch taken
		_expectedCyclesToUse++;

		if ((PC >> 8) != (address >> 8)) { // Crossed page boundary
			_cycles += 2;
			_expectedCyclesToUse += 2;
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

	// Then high nibble
	answer = (addend & 0xf0) + (operand & 0xf0);
	
	// Then combine them
	answer += a_low;

	// Then turn the result into BCD
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
					
	// Then high nibble
	operand = (operand & 0xf0) - (subtrahend & 0xf0);

	// Then combine them
	operand += op_l;

	// Then turn the result into BCD
	if (operand < 0) 
		operand -= 0x60;

	A = (Byte) operand & 0xff;

	setFlagZByValue(A);
	setFlagNByValue(A);
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
void MOS6502::ins_adc(const Byte opcode) {
	Byte operand = getData(opcode);
	
	if (Flags.D) {
		bcdADC(operand);
	} else { 
		doADC(operand);
	}
}

// AND
void MOS6502::ins_and(const Byte opcode) {
	Byte data = getData(opcode);
	A &= data;
	setFlagZByValue(A);
	setFlagNByValue(A);
}

// ASL
void MOS6502::ins_asl(const Byte opcode) {
	Word address;
	Byte data;
	
	getAorData(data, opcode, address);

	Flags.C = isNegative(data);
	data = data << 1;
	setFlagNByValue(data);
	setFlagZByValue(data);

	putAorData(data, opcode, address);
	
	_cycles++;
	if (_instructions.at(opcode).addrmode == AddressingMode::AbsoluteX)
		_cycles++;	
}

// BCC
void MOS6502::ins_bcc(const Byte opcode) {
	doBranch(!Flags.C, opcode);
}

// BCS
void MOS6502::ins_bcs(const Byte opcode) {
	doBranch(Flags.C, opcode);
}

// BEQ
void MOS6502::ins_beq(const Byte opcode) {
	doBranch(Flags.Z, opcode);
}

// BIT
void MOS6502::ins_bit(const Byte opcode) {
	Byte data;

	data = getData(opcode);
	setFlagZByValue(A & data);
	setFlagNByValue(data);
	// Copy bit 6 of the value into the V flag
	Flags.V = (data & (1 << 6)) != 0;
}

// BMI
void MOS6502::ins_bmi(const Byte opcode) {
	doBranch(Flags.N, opcode);
}

// BNE
void MOS6502::ins_bne(const Byte opcode) {
	doBranch(!Flags.Z, opcode);
}

// BPL
void MOS6502::ins_bpl(const Byte opcode) {
	doBranch(!Flags.N, opcode);
}

// BRK
void MOS6502::ins_brk([[maybe_unused]] const Byte opcode) {
	// push PC + 1 to the stack. See:
	// https://retrocomputing.stackexchange.com/questions/12291/what-are-uses-of-the-byte-after-brk-instruction-on-6502
	addBacktrace(PC - 1);
	PC++;
	_BRKCount++;
	interrupt();
	Flags.B = 1;
}

// BVC
void MOS6502::ins_bvc(const Byte opcode) {
	doBranch(!Flags.V, opcode);
}

// BVS
void MOS6502::ins_bvs(const Byte opcode) {
	doBranch(Flags.V, opcode);
}

// CLC
void MOS6502::ins_clc([[maybe_unused]] const Byte opcode) {
	Flags.C = 0;
	_cycles++;		// Single byte instruction
}

// CLD
void MOS6502::ins_cld([[maybe_unused]] const Byte opcode) {
	Flags.D = 0;
	_cycles++;		// Single byte instruction
}

// CLI
void MOS6502::ins_cli([[maybe_unused]] const Byte opcode) {
	Flags.I = 0;
	_cycles++;		// Single byte instruction
}

// CLV
void MOS6502::ins_clv([[maybe_unused]] const Byte opcode) {
	Flags.V = 0;
	_cycles++;		// Single byte instruction
}

// CMP
void MOS6502::ins_cmp(const Byte opcode) {
	Byte data = getData(opcode);

	Flags.C = A >= data;
	Flags.Z = A == data;

	Byte result = A - data;
	setFlagNByValue(result);
}

// CPX
void MOS6502::ins_cpx(const Byte opcode) {
	Byte data = getData(opcode);

	Flags.C = X >= data;
	Flags.Z = X == data;

	Byte result = X - data;
	setFlagNByValue(result);
}

// CPY
void MOS6502::ins_cpy(const Byte opcode) {
	Byte data = getData(opcode);
	
	Flags.C = Y >= data;
	Flags.Z = Y == data;

	Byte result = Y - data;
	setFlagNByValue(result);
}

// DEC
void MOS6502::ins_dec(const Byte opcode) {
	Word address;
	Byte data;

	address = getAddress(opcode);
	data = readByte(address);
	data--;
	writeByte(address, data);
	setFlagZByValue(data);
	setFlagNByValue(data);
	_cycles++;
	if (_instructions.at(opcode).addrmode == AddressingMode::AbsoluteX)
		_cycles++;
}

// DEX
void MOS6502::ins_dex([[maybe_unused]] const Byte opcode) {
	X--;
	setFlagNByValue(X);
	setFlagZByValue(X);
	_cycles++;
}

// DEY
void MOS6502::ins_dey([[maybe_unused]] const Byte opcode) {
	Y--;
	setFlagNByValue(Y);
	setFlagZByValue(Y);
	_cycles++;
}

// EOR
void MOS6502::ins_eor(const Byte opcode) {
	Byte data;

	data = getData(opcode);
	A ^= data;
	setFlagZByValue(A);
	setFlagNByValue(A);
}

// INC
void MOS6502::ins_inc(const Byte opcode) {
	Word address;
	Byte data;

	address = getAddress(opcode);
	data = readByte(address);
	data++;
	writeByte(address, data);
	setFlagZByValue(data);
	setFlagNByValue(data);
	_cycles++;
	if (_instructions.at(opcode).addrmode == AddressingMode::AbsoluteX)
		_cycles++;
}

// INX
void MOS6502::ins_inx([[maybe_unused]] const Byte opcode) {
	X++;
	setFlagZByValue(X);
	setFlagNByValue(X);
	_cycles++;
}

// INY
void MOS6502::ins_iny([[maybe_unused]] const Byte opcode) {
	Y++;
	setFlagZByValue(Y);
	setFlagNByValue(Y);
	_cycles++;
}

// JMP
void MOS6502::ins_jmp(const Byte opcode) {
	Word address = readWord(PC);
	bool indirect = _instructions.at(opcode).addrmode == AddressingMode::Indirect;
	
	if (indirect) {
		if ((address & 0xff) == 0xff) { // implement the JMP Indirect bug
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
void MOS6502::ins_jsr([[maybe_unused]] const Byte opcode) {
	addBacktrace(PC - 1);

	pushWord(PC + 1);
	PC = readWord(PC);
	_cycles++;
}

// LDA
void MOS6502::ins_lda(const Byte opcode) {
	A = getData(opcode);
	setFlagZByValue(A);
	setFlagNByValue(A);
}

// LDX
void MOS6502::ins_ldx(const Byte opcode) {
	X = getData(opcode);
	setFlagZByValue(X);
	setFlagNByValue(X);
}

// LDY
void MOS6502::ins_ldy(const Byte opcode) {
	Y = getData(opcode);
	setFlagZByValue(Y);
	setFlagNByValue(Y);
}

// LSR
void MOS6502::ins_lsr(const Byte opcode) {
	Word address;
	Byte data;

	getAorData(data, opcode, address);
	
	Flags.C = (data & 1);
	data = data >> 1;
	setFlagZByValue(data);
	setFlagNByValue(data);
	
	putAorData(data, opcode, address);

	_cycles++;
	if (_instructions.at(opcode).addrmode == AddressingMode::AbsoluteX)
		_cycles++;	
}

// NOP
void MOS6502::ins_nop([[maybe_unused]] const Byte opcode) {
	// NOP, like all single byte instructions, takes
	// two cycles.
	_cycles++;
}

// ORA
void MOS6502::ins_ora(const Byte opcode) {
	A |= getData(opcode);
	setFlagNByValue(A);
	setFlagZByValue(A);
}

// PHA
void MOS6502::ins_pha([[maybe_unused]] const Byte opcode) {
	push(A);
	_cycles++;		// Single byte instruction
}

// PHP
void MOS6502::ins_php([[maybe_unused]] const Byte opcode) {
	pushPS();
	_cycles++;		// Single byte instruction
}

// PLA
void MOS6502::ins_pla([[maybe_unused]] const Byte opcode) {
	A = pop();
	setFlagNByValue(A);
	setFlagZByValue(A);
	_cycles += 2;      
}

// PLP
void MOS6502::ins_plp([[maybe_unused]] const Byte opcode) {
	popPS();
	_cycles += 2;
}

// ROL
void MOS6502::ins_rol(const Byte opcode) {
	Word address;
	Byte data, oldCarryFlag;
	
	getAorData(data, opcode, address);
	oldCarryFlag = Flags.C;
	Flags.C = isNegative(data);

	data = (data << 1) | oldCarryFlag;

	setFlagZByValue(data);
	setFlagNByValue(data);

	putAorData(data, opcode, address);

	_cycles++;
	if (_instructions.at(opcode).addrmode == AddressingMode::AbsoluteX)
		_cycles++;
}

// ROR
void MOS6502::ins_ror(const Byte opcode) {
	Word address;
	Byte data, newCarryFlag;
	
	getAorData(data, opcode, address);

	newCarryFlag = data & 1;
	data = data >> 1;
	if (Flags.C)
		data |= NegativeBit;
	setFlagNByValue(data);
	setFlagZByValue(data);
	Flags.C = (newCarryFlag == 1);

	putAorData(data, opcode, address);

	_cycles++;
	if (_instructions.at(opcode).addrmode == AddressingMode::AbsoluteX)
		_cycles++;
}

// RTI
void MOS6502::ins_rti([[maybe_unused]] const Byte opcode) {
	removeBacktrace();
	popPS();
	PC = popWord();
	_cycles += 2;
}

// RTS
void MOS6502::ins_rts([[maybe_unused]] const Byte opcode) {
	removeBacktrace();
	PC = popWord() + 1;
	_cycles += 3;	       
}

// SBC
void MOS6502::ins_sbc(const Byte opcode) {
	Byte operand = getData(opcode);

	if (Flags.D) {
		bcdSBC(operand); 
	} else {	
		doADC(~operand);
	}
}

// SEC
void MOS6502::ins_sec([[maybe_unused]] const Byte opcode) {
	Flags.C = 1;
	_cycles++;		// Single byte instruction
}

// SED
void MOS6502::ins_sed([[maybe_unused]] const Byte opcode) {
	Flags.D = 1;
	_cycles++;		// Single byte instruction
}

// SEI
void MOS6502::ins_sei([[maybe_unused]] const Byte opcode) {
	Flags.I = 1;
	_cycles++;		// Single byte instruction
}

// STA
void MOS6502::ins_sta(const Byte opcode) {
	Word address = getAddress(opcode);
	writeByte(address, A);
}

// STX
void MOS6502::ins_stx(const Byte opcode) {
	Word address = getAddress(opcode);
	writeByte(address, X);
}

// STY
void MOS6502::ins_sty(const Byte opcode) {
	Word address = getAddress(opcode);
	writeByte(address, Y);
}

// TAX
void MOS6502::ins_tax([[maybe_unused]] const Byte opcode) {
	X = A;
	setFlagZByValue(X);
	setFlagNByValue(X);
	_cycles++;
}

// TAY
void MOS6502::ins_tay([[maybe_unused]] const Byte opcode) {
	Y = A;
	setFlagZByValue(Y);
	setFlagNByValue(Y);
	_cycles++;
}

// TSX
void MOS6502::ins_tsx([[maybe_unused]] const Byte opcode) {
	X = SP;
	setFlagZByValue(X);
	setFlagNByValue(X);
	_cycles++;
}

// TXA
void MOS6502::ins_txa([[maybe_unused]] const Byte opcode) {
	A = X;
	setFlagZByValue(A);
	setFlagNByValue(A);
	_cycles++;
}

// TXS
void MOS6502::ins_txs([[maybe_unused]] const Byte opcode) {
	SP = X;
	_cycles++;
}

// TYA
void MOS6502::ins_tya([[maybe_unused]] const Byte opcode) {
	A = Y;
	setFlagZByValue(A);
	setFlagNByValue(A);
	_cycles++;
}
