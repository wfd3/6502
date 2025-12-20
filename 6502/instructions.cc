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
	bool accumulator = instructionIsAddressingMode(opcode, AddressingMode::Accumulator);

	if (accumulator)
		data = _ctx.A;
	else {
		address = getAddress(opcode);
		data = readByte(address);
	}
}

void MOS6502::putAorData(const Byte data, const Byte opcode, Word address) {
	bool accumulator = instructionIsAddressingMode(opcode, AddressingMode::Accumulator);

	if (accumulator)
		_ctx.A = data;
	else 
		writeByte(address, data);
}

// Set PC to @address if @condition is true
void MOS6502::doBranch(const bool condition, const Byte opcode) {
	Word address = getAddress(opcode);

	if (condition) {
		_ctx.branchTaken = true;
		_ctx.PC = address;
	}
}

// BCD addition and subtraction functions.
// See:
// https://www.electrical4u.com/bcd-or-binary-coded-decimal-bcd-conversion-addition-subtraction/
void MOS6502::bcdADC(const Byte operand) {
	Byte addend, carry, a_low;
	int answer;

	addend = _ctx.A;
	carry = _ctx.Flags.C;

	// Low nibble first
	a_low = static_cast<Byte>((addend & 0x0f) + (operand & 0x0f) + carry);
	if (a_low >= 0x0a) 
		a_low = ((a_low + 0x06) & 0x0f) + 0x10;

	// Then high nibble
	answer = (addend & 0xf0) + (operand & 0xf0);
	
	// Then combine them
	answer += a_low;

	// Then turn the result into BCD
	if (answer >= 0xa0) 
		answer += 0x60;
	
	_ctx.A = static_cast<Word>(answer & 0xff);
	
	setFlagNByValue(_ctx.A);
	setFlagZByValue(_ctx.A);
	_ctx.Flags.C = (answer >= 0x100);
	_ctx.Flags.V = (answer < -128) || (answer > 127);
}

void MOS6502::bcdSBC(const Byte subtrahend) {
	SByte op_l;
	int operand;
	Byte carry;

	operand = _ctx.A;
	carry = (_ctx.Flags.C == 0);

	// Low nibble first
	op_l = static_cast<SByte>((operand & 0x0f) - (subtrahend & 0x0f) - carry);
	if (op_l < 0) 
		op_l = ((op_l - 0x06) & 0x0f) - 0x10;
					
	// Then high nibble
	operand = (operand & 0xf0) - (subtrahend & 0xf0);

	// Then combine them
	operand += op_l;

	// Then turn the result into BCD
	if (operand < 0) 
		operand -= 0x60;

	_ctx.A = static_cast<Byte>(operand & 0xff);

	setFlagZByValue(_ctx.A);
	setFlagNByValue(_ctx.A);
	_ctx.Flags.C = (operand >= 0);
}

// A = A + operand + Flags.C
void MOS6502::doADC(const Byte operand) {
	Word result;
	bool same_sign;

	same_sign = isNegative(_ctx.A) == isNegative(operand);
	result = _ctx.A + operand + _ctx.Flags.C;
	_ctx.A = result & 0xff;
	setFlagZByValue(_ctx.A);
	setFlagNByValue(_ctx.A);
	_ctx.Flags.C = result > 0xff;
	_ctx.Flags.V = same_sign && (isNegative(_ctx.A) != isNegative(operand));
}

////
// CPU Instructions

// ADC
void MOS6502::ins_adc(const Byte opcode) {
	Byte operand = getData(opcode);
	
	if (_ctx.Flags.D) {
		bcdADC(operand);
	} else { 
		doADC(operand);
	}
}

// AND
void MOS6502::ins_and(const Byte opcode) {
	Byte data = getData(opcode);
	_ctx.A &= data;
	setFlagZByValue(_ctx.A);
	setFlagNByValue(_ctx.A);
}

// ASL
void MOS6502::ins_asl(const Byte opcode) {
	Word address;
	Byte data;
	
	getAorData(data, opcode, address);

	_ctx.Flags.C = isNegative(data);
	data = data << 1;
	setFlagNByValue(data);
	setFlagZByValue(data);

	putAorData(data, opcode, address);
	

}

// BCC
void MOS6502::ins_bcc(const Byte opcode) {
	doBranch(!_ctx.Flags.C, opcode);
}

// BCS
void MOS6502::ins_bcs(const Byte opcode) {
	doBranch(_ctx.Flags.C, opcode);
}

// BEQ
void MOS6502::ins_beq(const Byte opcode) {
	doBranch(_ctx.Flags.Z, opcode);
}

// BIT
void MOS6502::ins_bit(const Byte opcode) {
	Byte data;

	data = getData(opcode);
	setFlagZByValue(_ctx.A & data);
	setFlagNByValue(data);
	// Copy bit 6 of the value into the V flag
	_ctx.Flags.V = (data & (1 << 6)) != 0;
}

// BMI
void MOS6502::ins_bmi(const Byte opcode) {
	doBranch(_ctx.Flags.N, opcode);
}

// BNE
void MOS6502::ins_bne(const Byte opcode) {
	doBranch(!_ctx.Flags.Z, opcode);
}

// BPL
void MOS6502::ins_bpl(const Byte opcode) {
	doBranch(!_ctx.Flags.N, opcode);
}

// BRK
void MOS6502::ins_brk([[maybe_unused]] const Byte opcode) {
	debugger.addBacktrace(_ctx.PC - 1);
	_BRKCount++;
	// push PC + 1 to the stack. See:
	// https://retrocomputing.stackexchange.com/questions/12291/what-are-uses-of-the-byte-after-brk-instruction-on-6502
	_ctx.PC++;
	interrupt(INTERRUPT_VECTOR);
	_ctx.Flags.B = 1;
}

// BVC
void MOS6502::ins_bvc(const Byte opcode) {
	doBranch(!_ctx.Flags.V, opcode);
}

// BVS
void MOS6502::ins_bvs(const Byte opcode) {
	doBranch(_ctx.Flags.V, opcode);
}

// CLC
void MOS6502::ins_clc([[maybe_unused]] const Byte opcode) {
	_ctx.Flags.C = 0;
		// Single byte instruction
}

// CLD
void MOS6502::ins_cld([[maybe_unused]] const Byte opcode) {
	_ctx.Flags.D = 0;
		// Single byte instruction
}

// CLI
void MOS6502::ins_cli([[maybe_unused]] const Byte opcode) {
	_ctx.Flags.I = 0;
		// Single byte instruction
}

// CLV
void MOS6502::ins_clv([[maybe_unused]] const Byte opcode) {
	_ctx.Flags.V = 0;
		// Single byte instruction
}

// CMP
void MOS6502::ins_cmp(const Byte opcode) {
	Byte data = getData(opcode);

	_ctx.Flags.C = _ctx.A >= data;
	_ctx.Flags.Z = _ctx.A == data;

	Byte result = _ctx.A - data;
	setFlagNByValue(result);
}

// CPX
void MOS6502::ins_cpx(const Byte opcode) {
	Byte data = getData(opcode);

	_ctx.Flags.C = _ctx.X >= data;
	_ctx.Flags.Z = _ctx.X == data;

	Byte result = _ctx.X - data;
	setFlagNByValue(result);
}

// CPY
void MOS6502::ins_cpy(const Byte opcode) {
	Byte data = getData(opcode);
	
	_ctx.Flags.C = _ctx.Y >= data;
	_ctx.Flags.Z = _ctx.Y == data;

	Byte result = _ctx.Y - data;
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

}

// DEX
void MOS6502::ins_dex([[maybe_unused]] const Byte opcode) {
	_ctx.X--;
	setFlagNByValue(_ctx.X);
	setFlagZByValue(_ctx.X);

}

// DEY
void MOS6502::ins_dey([[maybe_unused]] const Byte opcode) {
	_ctx.Y--;
	setFlagNByValue(_ctx.Y);
	setFlagZByValue(_ctx.Y);

}

// EOR
void MOS6502::ins_eor(const Byte opcode) {
	Byte data;

	data = getData(opcode);
	_ctx.A ^= data;
	setFlagZByValue(_ctx.A);
	setFlagNByValue(_ctx.A);
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

}

// INX
void MOS6502::ins_inx([[maybe_unused]] const Byte opcode) {
	_ctx.X++;
	setFlagZByValue(_ctx.X);
	setFlagNByValue(_ctx.X);

}

// INY
void MOS6502::ins_iny([[maybe_unused]] const Byte opcode) {
	_ctx.Y++;
	setFlagZByValue(_ctx.Y);
	setFlagNByValue(_ctx.Y);

}

// JMP
void MOS6502::ins_jmp(const Byte opcode) {
	Word address = readWord(_ctx.PC);
	
	if (instructionIsAddressingMode(opcode, AddressingMode::Indirect)) {
		if ((address & 0xff) == 0xff) { // implement the JMP Indirect bug
			Byte lsb = readByte(address);
			Byte msb = readByte(address & 0xff00);
			address = (msb << 8) | lsb;
		} else {
			address = readWord(address);
		}
	} 

	_ctx.PC = address;
}

// JSR
void MOS6502::ins_jsr([[maybe_unused]] const Byte opcode) {
	debugger.addBacktrace(_ctx.PC - 1);

	pushWord(_ctx.PC + 1);
	_ctx.PC = readWord(_ctx.PC);

}

// LDA
void MOS6502::ins_lda(const Byte opcode) {
	_ctx.A = getData(opcode);
	setFlagZByValue(_ctx.A);
	setFlagNByValue(_ctx.A);
}

// LDX
void MOS6502::ins_ldx(const Byte opcode) {
	_ctx.X = getData(opcode);
	setFlagZByValue(_ctx.X);
	setFlagNByValue(_ctx.X);
}

// LDY
void MOS6502::ins_ldy(const Byte opcode) {
	_ctx.Y = getData(opcode);
	setFlagZByValue(_ctx.Y);
	setFlagNByValue(_ctx.Y);
}

// LSR
void MOS6502::ins_lsr(const Byte opcode) {
	Word address;
	Byte data;

	getAorData(data, opcode, address);

	_ctx.Flags.C = (data & 1); // Bit 1 of data becomes Carry
	data = data >> 1;
	setFlagZByValue(data);
	setFlagNByValue(data);
	
	putAorData(data, opcode, address);


}

// NOP
void MOS6502::ins_nop([[maybe_unused]] const Byte opcode) {
	// NOP, like all single byte instructions, takes two cycles.

}

// ORA
void MOS6502::ins_ora(const Byte opcode) {
	_ctx.A |= getData(opcode);
	setFlagNByValue(_ctx.A);
	setFlagZByValue(_ctx.A);
}

// PHA
void MOS6502::ins_pha([[maybe_unused]] const Byte opcode) {
	push(_ctx.A);
		// Single byte instruction
}

// PHP
void MOS6502::ins_php([[maybe_unused]] const Byte opcode) {
	pushPS();
		// Single byte instruction
}

// PLA
void MOS6502::ins_pla([[maybe_unused]] const Byte opcode) {
	_ctx.A = pop();
	setFlagNByValue(_ctx.A);
	setFlagZByValue(_ctx.A);
      
}

// PLP
void MOS6502::ins_plp([[maybe_unused]] const Byte opcode) {
	popPS();

}

// ROL
void MOS6502::ins_rol(const Byte opcode) {
	Word address;
	Byte data, oldCarryFlag;
	
	getAorData(data, opcode, address);
	oldCarryFlag = _ctx.Flags.C;
	_ctx.Flags.C = isNegative(data);

	data = (data << 1) | oldCarryFlag; // Carry becomes bit 1 of result

	setFlagZByValue(data);
	setFlagNByValue(data);

	putAorData(data, opcode, address);


}

// ROR
void MOS6502::ins_ror(const Byte opcode) {
	Word address;
	Byte data, newCarryFlag;
	
	getAorData(data, opcode, address);

	newCarryFlag = data & 1;
	data = data >> 1;
	data |=  _ctx.Flags.C << 7;  // Carry bit becomes bit 7 of the result
	setFlagNByValue(data);
	setFlagZByValue(data);
	_ctx.Flags.C = (newCarryFlag == 1);

	putAorData(data, opcode, address);


}

// RTI
void MOS6502::ins_rti([[maybe_unused]] const Byte opcode) {
	debugger.removeBacktrace();
	popPS();
	_ctx.PC = popWord();

}

// RTS
void MOS6502::ins_rts([[maybe_unused]] const Byte opcode) {
	debugger.removeBacktrace();
	_ctx.PC = popWord() + 1;
	       
}

// SBC
void MOS6502::ins_sbc(const Byte opcode) {
	Byte operand = getData(opcode);

	if (_ctx.Flags.D) {
		bcdSBC(operand); 
	} else {	
		doADC(~operand);
	}
}

// SEC
void MOS6502::ins_sec([[maybe_unused]] const Byte opcode) {
	_ctx.Flags.C = 1;
		// Single byte instruction
}

// SED
void MOS6502::ins_sed([[maybe_unused]] const Byte opcode) {
	_ctx.Flags.D = 1;
		// Single byte instruction
}

// SEI
void MOS6502::ins_sei([[maybe_unused]] const Byte opcode) {
	_ctx.Flags.I = 1;
		// Single byte instruction
}

// STA
void MOS6502::ins_sta(const Byte opcode) {
	Word address = getAddress(opcode);
	writeByte(address, _ctx.A);

	// All other instances of (Indirect),Y are N cycles, plus 1 if the address calculation crosses a
	// page boundary.  STA (Indirect),Y is 6 cycles regardless of page boundaries.
	// This is handled in the instruction map with maxCycles=6 and no PageBoundary flag.
}

// STX
void MOS6502::ins_stx(const Byte opcode) {
	Word address = getAddress(opcode);
	writeByte(address, _ctx.X);
}

// STY
void MOS6502::ins_sty(const Byte opcode) {
	Word address = getAddress(opcode);
	writeByte(address, _ctx.Y);
}

// TAX
void MOS6502::ins_tax([[maybe_unused]] const Byte opcode) {
	_ctx.X = _ctx.A;
	setFlagZByValue(_ctx.X);
	setFlagNByValue(_ctx.X);

}

// TAY
void MOS6502::ins_tay([[maybe_unused]] const Byte opcode) {
	_ctx.Y = _ctx.A;
	setFlagZByValue(_ctx.Y);
	setFlagNByValue(_ctx.Y);

}

// TSX
void MOS6502::ins_tsx([[maybe_unused]] const Byte opcode) {
	_ctx.X = _ctx.SP;
	setFlagZByValue(_ctx.X);
	setFlagNByValue(_ctx.X);

}

// TXA
void MOS6502::ins_txa([[maybe_unused]] const Byte opcode) {
	_ctx.A = _ctx.X;
	setFlagZByValue(_ctx.A);
	setFlagNByValue(_ctx.A);

}

// TXS
void MOS6502::ins_txs([[maybe_unused]] const Byte opcode) {
	_ctx.SP = _ctx.X;

}

// TYA
void MOS6502::ins_tya([[maybe_unused]] const Byte opcode) {
	_ctx.A = _ctx.Y;
	setFlagZByValue(_ctx.A);
	setFlagNByValue(_ctx.A);

}
