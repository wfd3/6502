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

// BCD addition and substraction functions.
// See:
// https://www.electrical4u.com/bcd-or-binary-coded-decimal-bcd-conversion-addition-subtraction/

void CPU::bcdADC(Byte operand) {
	Byte addend, carry, a_low;
	int answer;

	addend = A;
	carry = Flags.C;

	// Low nibble first
	a_low = (addend & 0x0f) + (operand & 0x0f) + carry;
	if (a_low >= 0x0a) 
		a_low = ((a_low + 0x06) & 0x0f) + 0x10;

	answer = (addend & 0xf0) + (operand & 0xf0) + a_low;
	if (answer >= 0xa0) 
		answer += 0x60;
	
	A = answer & 0xff;
	
	setFlagN(answer);
	setFlagZ(answer);
	Flags.C = (answer >= 0x100);
	Flags.V = (answer < -128) || (answer > 127);
}

void CPU::bcdSBC(Byte subtrahend) {
	SByte op_l;
	int operand;
	Byte carry;

	operand = A;
	carry = (Flags.C == 0);

	// Low nibble first
	op_l = (operand & 0x0f) - (subtrahend & 0x0f) - carry;
	if (op_l < 0) 
		op_l = ((op_l - 0x06) & 0x0f) - 0x10;
					
	operand = (operand & 0xf0) - (subtrahend & 0xf0);
	operand += op_l;
	if (operand < 0) 
		operand -= 0x60;

	A = (Byte) operand & 0xff;

	setFlagZ(A);
	Flags.C = (operand >= 0);
}

// A = A + operand + Flags.C
void CPU::doADC(Byte operand) {
	Word result;
	bool same_sign;

	same_sign = isNegative(A) == isNegative(operand);
	result = A + operand + Flags.C;
	A = result & 0xff;
	setFlagZ(A);
	setFlagN(A);
	Flags.C = result > 0xff;
	Flags.V = same_sign && (isNegative(A) != isNegative(operand));
}

////
// CPU Instructions

// ADC
void CPU::ins_adc(Byte opcode, Byte &expectedCyclesToUse) {
	Byte operand = getData(opcode, expectedCyclesToUse);
	
	if (Flags.D) {
		bcdADC(operand);
		return;
	}

	doADC(operand);
}

// AND
void CPU::ins_and(Byte opcode, Byte &expectedCyclesToUse) {
	Byte data;

	data = getData(opcode, expectedCyclesToUse);
	A &= data;
	setFlagZ(A);
	setFlagN(A);
}

// ASL
void CPU::ins_asl(Byte opcode, Byte &expectedCyclesToUse) {
	Word address;
	Byte data;

	if (instructions[opcode].addrmode == ADDR_MODE_ACC) {
		data = A;
	} else {
		address = getAddress(opcode, expectedCyclesToUse);
		data = readByte(address);
	}

	Flags.C = isNegative(data);
	data = data << 1;
	setFlagN(data);
	setFlagZ(data);

	if (instructions[opcode].addrmode == ADDR_MODE_ACC) {
		A = data;
	} else {
		writeByte(address, data);
	}
	
	Cycles++;
	if (instructions[opcode].addrmode == ADDR_MODE_ABX)
		Cycles++;	
}

// Set PC to @address if @condition is true
void CPU::doBranch(bool condition,  Byte &expectedCyclesToUse) {
	if (condition) {
		Cycles++;	// Branch taken
		expectedCyclesToUse++;

		if ((PC >> 8) != (address >> 8)) { // Crossed page boundry
			Cycles += 2;
			expectedCyclesToUse += 2;
		}

		PC = address;
	}
}

// BCC
void CPU::ins_bcc(Byte opcode, Byte &expectedCyclesToUse) {
	Word address;

	address = getAddress(opcode, expectedCyclesToUse);
	doBranch(!Flags.C, address, expectedCyclesToUse);
}

// BCS
void CPU::ins_bcs(Byte opcode, Byte &expectedCyclesToUse) {
	Word address;

	address = getAddress(opcode, expectedCyclesToUse);
	doBranch(Flags.C, address, expectedCyclesToUse);
}

// BEQ
void CPU::ins_beq(Byte opcode, Byte &expectedCyclesToUse) {
	Word address;

	address = getAddress(opcode, expectedCyclesToUse);
	doBranch(Flags.Z, address, expectedCyclesToUse);
}

// BIT
void CPU::ins_bit(Byte opcode, Byte &expectedCyclesToUse) {
	Byte data;

	data = getData(opcode, expectedCyclesToUse);
	setFlagZ(A & data);
	setFlagN(data);
	Flags.V = (data & (1 << 6)) != 0;
}

// BMI
void CPU::ins_bmi(Byte opcode, Byte &expectedCyclesToUse) {
	Word address;

	address = getAddress(opcode, expectedCyclesToUse);
	doBranch(Flags.N, address, expectedCyclesToUse);
}

// BNE
void CPU::ins_bne(Byte opcode, Byte &expectedCyclesToUse) {
	Word address;

	address = getAddress(opcode, expectedCyclesToUse);
	doBranch(!Flags.Z, address, expectedCyclesToUse);
}

// BPL
void CPU::ins_bpl(Byte opcode, Byte &expectedCyclesToUse) {
	Word address;

	address = getAddress(opcode, expectedCyclesToUse);
	doBranch(!Flags.N, address, expectedCyclesToUse);
}

// BRK
void CPU::ins_brk(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;

	// From the internets, BRK pushes PC + 1 to the stack. See:
	// https://retrocomputing.stackexchange.com/questions/12291/what-are-uses-of-the-byte-after-brk-instruction-on-6502

	PC++;
	pushWord(PC);

	addBacktrace(PC);

	pushPS();
	PC = readWord(INTERRUPT_VECTOR);
	Flags.B = 1;
	Flags.I = 1;
	Cycles++;
}

// BVC
void CPU::ins_bvc(Byte opcode, Byte &expectedCyclesToUse) {
	Word address;

	address = getAddress(opcode, expectedCyclesToUse);
	doBranch(!Flags.V, address, expectedCyclesToUse);
}

// BVS
void CPU::ins_bvs(Byte opcode, Byte &expectedCyclesToUse) {
	Word address;

	address = getAddress(opcode, expectedCyclesToUse);
	doBranch(Flags.V, address, expectedCyclesToUse);
}

// CLC
void CPU::ins_clc(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	Flags.C = 0;
	Cycles++;		// Single byte instruction
}

// CLD
void CPU::ins_cld(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	Flags.D = 0;
	Cycles++;		// Single byte instruction
}

// CLI
void CPU::ins_cli(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	Flags.I = 0;
	Cycles++;		// Single byte instruction
}

// CLV
void CPU::ins_clv(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	Flags.V = 0;
	Cycles++;		// Single byte instruction
}

// CMP
void CPU::ins_cmp(Byte opcode, Byte &expectedCyclesToUse) {
	Byte data;

	data = getData(opcode, expectedCyclesToUse);
	Flags.C = A >= data;
	Flags.Z = A == data;
	Flags.N = A  < data;
}

// CPX
void CPU::ins_cpx(Byte opcode, Byte &expectedCyclesToUse) {
	Byte data;

	data = getData(opcode, expectedCyclesToUse);
	Flags.C = X >= data;
	Flags.Z = X == data;
	Flags.N = X  < data;
}

// CPY
void CPU::ins_cpy(Byte opcode, Byte &expectedCyclesToUse) {
	Byte data;

	data = getData(opcode, expectedCyclesToUse);
	Flags.C = Y >= data;
	Flags.Z = Y == data;
	Flags.N = Y  < data;

}

// DEC
void CPU::ins_dec(Byte opcode, Byte &expectedCyclesToUse) {
	Word address;
	Byte data;

	address = getAddress(opcode, expectedCyclesToUse);
	data = readByte(address);
	data--;
	writeByte(address, data);
	setFlagZ(data);
	setFlagN(data);
	Cycles++;
	if (instructions[opcode].addrmode == ADDR_MODE_ABX)
		Cycles++;
}

// DEX
void CPU::ins_dex(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	X--;
	setFlagN(X);
	setFlagZ(X);
	Cycles++;
}

// DEY
void CPU::ins_dey(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	Y--;
	setFlagN(Y);
	setFlagZ(Y);
	Cycles++;
}

// EOR
void CPU::ins_eor(Byte opcode, Byte &expectedCyclesToUse) {
	Byte data;

	data = getData(opcode, expectedCyclesToUse);
	A ^= data;
	setFlagZ(A);
	setFlagN(A);
}

// INC
void CPU::ins_inc(Byte opcode, Byte &expectedCyclesToUse) {
	Word address;
	Byte data;

	address = getAddress(opcode, expectedCyclesToUse);
	data = readByte(address);
	data++;
	writeByte(address, data);
	setFlagZ(data);
	setFlagN(data);
	Cycles++;
	if (instructions[opcode].addrmode == ADDR_MODE_ABX)
		Cycles++;
}

// INX
void CPU::ins_inx(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	X++;
	setFlagZ(X);
	setFlagN(X);
	Cycles++;
}

// INY
void CPU::ins_iny(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	Y++;
	setFlagZ(Y);
	setFlagN(Y);
	Cycles++;
}

// JMP
void CPU::ins_jmp(Byte opcode, Byte &expectedCyclesToUse) {
	Word address = PC;

	addBacktrace(PC - 1);

	if (instructions[opcode].addrmode == ADDR_MODE_IND) {
		address = getAddress(opcode, expectedCyclesToUse);
	}

	PC = readWord(address);
}

// JSR
void CPU::ins_jsr(Byte opcode, Byte &expectedCyclesToUse) {
	Word newPC;
	
	addBacktrace(PC - 1);

	newPC = readWord(PC);
	pushWord(PC + 1);
	PC = newPC;
	
	Cycles++;

	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
}

// LDA
void CPU::ins_lda(Byte opcode, Byte &expectedCyclesToUse) {
	A = getData(opcode, expectedCyclesToUse);
	setFlagZ(A);
	setFlagN(A);
}

// LDX
void CPU::ins_ldx(Byte opcode, Byte &expectedCyclesToUse) {
	X = getData(opcode, expectedCyclesToUse);
	setFlagZ(X);
	setFlagN(X);
}

// LDY
void CPU::ins_ldy(Byte opcode, Byte &expectedCyclesToUse) {
	Y = getData(opcode, expectedCyclesToUse);
	setFlagZ(Y);
	setFlagN(Y);
}

// LSR
void CPU::ins_lsr(Byte opcode, Byte &expectedCyclesToUse) {
	Word address;
	Byte data;

	if (instructions[opcode].addrmode == ADDR_MODE_ACC) 
		data = A;
	else {
		address = getAddress(opcode, expectedCyclesToUse);
		data = readByte(address);
	}

	Flags.C = (data & 1);
	data = data >> 1;
	setFlagZ(data);
	setFlagN(data);

	if (instructions[opcode].addrmode == ADDR_MODE_ACC)
		A = data;
	else 
		writeByte(address, data);

	Cycles++;
	if (instructions[opcode].addrmode == ADDR_MODE_ABX)
		Cycles++;	
}

// NOP
void CPU::ins_nop(Byte opcode, Byte &expectedCyclesToUse) {
	// NOP, like all single byte instructions, takes
	// two cycles.
	Cycles++;

	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
}

// ORA
void CPU::ins_ora(Byte opcode, Byte &expectedCyclesToUse) {
	Byte data;

	data = getData(opcode, expectedCyclesToUse);
	A |= data;
	setFlagN(A);
	setFlagZ(A);
}

// PHA
void CPU::ins_pha(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	push(A);
	Cycles++;		// Single byte instruction
}

// PLA
void CPU::ins_pla(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	A = pop();
	setFlagN(A);
	setFlagZ(A);
	Cycles += 2;      
}

// PHP
void CPU::ins_php(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;

	pushPS();
	Cycles++;		// Single byte instruction
}

// PLP
void CPU::ins_plp(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	popPS();
	Cycles += 2;
}

// ROL
void CPU::ins_rol(Byte opcode, Byte &expectedCyclesToUse) {
	Word address;
	Byte data, carry;

	if (instructions[opcode].addrmode == ADDR_MODE_ACC) {
		data = A;
	} else {
		address = getAddress(opcode, expectedCyclesToUse);
		data = readByte(address);
	}

	carry = Flags.C;
	Flags.C = isNegative(data);

	data = (data << 1) | carry;

	setFlagZ(data);
	setFlagN(data);

	if (instructions[opcode].addrmode == ADDR_MODE_ACC)
		A = data;
	else 
		writeByte(address, data);

	Cycles++;
	if (instructions[opcode].addrmode == ADDR_MODE_ABX)
		Cycles++;
}

// ROR
void CPU::ins_ror(Byte opcode, Byte &expectedCyclesToUse) {
	Word address;
	Byte data, zero;

	if (instructions[opcode].addrmode == ADDR_MODE_ACC) 
		data = A;
	else {
		address = getAddress(opcode, expectedCyclesToUse);
		data = readByte(address);
	}

	zero = data & 1;
	data = data >> 1;
	if (Flags.C)
		data |= NegativeBit;
	setFlagN(data);
	setFlagZ(data);
	Flags.C = zero;

	if (instructions[opcode].addrmode == ADDR_MODE_ACC)
		A = data;
	else 
		writeByte(address, data);

	Cycles++;
	if (instructions[opcode].addrmode == ADDR_MODE_ABX)
		Cycles++;
}

// RTI
void CPU::ins_rti(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;

	removeBacktrace();
	popPS();
	PC = popWord();
	Cycles += 2;
}

// RTS
void CPU::ins_rts(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;

	removeBacktrace();
	
	PC = popWord() + 1;
	Cycles += 3;	       
}

// SBC
void CPU::ins_sbc(Byte opcode, Byte &expectedCyclesToUse) {
	Byte operand = getData(opcode, expectedCyclesToUse);

	if (Flags.D) {
		bcdSBC(operand); 
		return;
	}
	
	doADC(~operand);
}

// SEC
void CPU::ins_sec(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	Flags.C = 1;
	Cycles++;		// Single byte instruction
}

// SED
void CPU::ins_sed(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	Flags.D = 1;
	Cycles++;		// Single byte instruction
}

// SEI
void CPU::ins_sei(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	Flags.I = 1;
	Cycles++;		// Single byte instruction
}

// STA
void CPU::ins_sta(Byte opcode, Byte &expectedCyclesToUse) {
	Word address = getAddress(opcode, expectedCyclesToUse);
	writeByte(address, A);
}

// STX
void CPU::ins_stx(Byte opcode, Byte &expectedCyclesToUse) {
	Word address = getAddress(opcode, expectedCyclesToUse);
	writeByte(address, X);
}

// STY
void CPU::ins_sty(Byte opcode, Byte &expectedCyclesToUse) {
	Word address = getAddress(opcode, expectedCyclesToUse);
	writeByte(address, Y);
}

// TAX
void CPU::ins_tax(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	X = A;
	setFlagZ(X);
	setFlagN(X);
	Cycles++;
}

// TAY
void CPU::ins_tay(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	Y = A;
	setFlagZ(Y);
	setFlagN(Y);
	Cycles++;
}

// TSX
void CPU::ins_tsx(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	X = SP;
	setFlagZ(X);
	setFlagN(X);
	Cycles++;
}

// TXA
void CPU::ins_txa(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	A = X;
	setFlagZ(A);
	setFlagN(A);
	Cycles++;
}

// TXS
void CPU::ins_txs(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	SP = X;
	Cycles++;
}

// TYA
void CPU::ins_tya(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	A = Y;
	setFlagZ(A);
	setFlagN(A);
	Cycles++;
}
