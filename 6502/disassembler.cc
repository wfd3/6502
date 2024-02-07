//
// Disassembler for emulated 6502.
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

#include <string>

#include <fmt/format.h>
#include <fmt/core.h>

#include <memory>
#include <6502.h>
#include <utils.h>

void MOS6502::decodeArgs(const bool atPC, const Byte ins, std::string& disassembly, 
					 std::string& opcodes, std::string& address, std::string& computedAddr) {

	auto mode = _instructions.at(ins).addrmode;
	Byte byteval;
	Word wordval;
	Byte rel;
	std::string out, addr, label;

	switch (mode) {
	case AddressingMode::Implied:
		break;

	case AddressingMode::Accumulator:
		disassembly = "A";
		address = "";
		break;

	case AddressingMode::Immediate:  // #$xx
		byteval = readByteAtPC();
		disassembly = fmt::format("#${:02x}", byteval);
		opcodes += fmt::format("{:02x} ", byteval);
		address = "";
		break;

	case AddressingMode::ZeroPage:  // $xx
		byteval = readByteAtPC();
		label = addressLabelSearch(byteval);
		addr = fmt::format("${:02x}", byteval);
	
		if (!label.empty()) {
			disassembly = label;
			address = addr;
		} else {
			disassembly = addr;
			address = "";
		}
		opcodes += fmt::format("{:02x} ", byteval);
		break;

	case AddressingMode::ZeroPageX:  // $xx,X
		byteval = readByteAtPC();
		label = addressLabelSearch(byteval);
		addr = fmt::format("${:04x}", byteval);

		if (!label.empty()) {
			disassembly = label;
			address = addr;
		} else {
			disassembly = addr;
			address = "";
		}
		disassembly += ",X";
		opcodes += fmt::format("{:02x} ", byteval);
		if (atPC) 
			computedAddr = fmt::format("${:04x}", byteval + X);
		break;

	case AddressingMode::ZeroPageY:  // $xx,Y
		byteval = readByteAtPC();
		label = addressLabelSearch(byteval);
		addr = fmt::format("${:04x}", byteval);
		if (!label.empty()) {
			disassembly = label;
			address = addr;
		} else {
			disassembly = addr;
			address = "";
		}
		disassembly += ",Y";
		opcodes += fmt::format("{:02x} ", byteval);
		if (atPC) 
			computedAddr = fmt::format("${:04x}", byteval + Y);
		break;

	case AddressingMode::Relative:
		rel = readByteAtPC();
		wordval = PC + SByte(rel);
		addr = fmt::format("${:04x}", wordval);
		label = addressLabel(wordval);

		if (!label.empty()) {
			disassembly = label;
			address = addr;
		} else { 
			disassembly = fmt::format("#${:02x}", rel);
			address = addr;
		} 
		opcodes += fmt::format("{:02x} ", rel);
		break;

	case AddressingMode::Absolute:  // $xxxx
		wordval = readWordAtPC();
		label = addressLabel(wordval);
		addr = fmt::format("${:04x}", wordval);

		if (!label.empty()) {
			disassembly = label;
			address = address;
		} else {
			disassembly = addr;
			address = "";
		}
		opcodes += fmt::format("{:02x} {:02x}", wordval & 0xff, (wordval >> 8) & 0xff);
		break;

	case AddressingMode::AbsoluteX:  // $xxxx,X
		wordval = readWordAtPC();
		label = addressLabelSearch(wordval);
		addr = fmt::format("${:04x}", wordval);
		if (!label.empty())  {
			disassembly = label;
			address = addr;
		} else { 
			disassembly = addr;
			address = "";
		}
		disassembly += ",X";
		opcodes += fmt::format("{:02x} {:02x}", 
							  wordval & 0xff, (wordval >> 8) & 0xff);
		computedAddr = fmt::format("${:04x}", wordval + X);
	
		break;		

	case AddressingMode::AbsoluteY:  // $xxxx,Y
		wordval = readWordAtPC();
		label = addressLabelSearch(wordval);
		addr = fmt::format("${:04x}", wordval);
		if (!label.empty()) {
			disassembly = label;
			address = addr;
		} else {
			disassembly = addr;
			address = "";
		}
		disassembly += ",Y";
		opcodes += fmt::format("{:02x} {:02x}", 
							  wordval & 0xff, (wordval >> 8) & 0xff);
		if (atPC) 
			computedAddr = fmt::format("${:04x}", wordval + Y);
		break;
		
	case AddressingMode::Indirect:  // $(xxxx)
		wordval = readWordAtPC();

		label = addressLabel(wordval);
		addr = fmt::format("{:04x}", wordval);
		if (!label.empty()) {
			disassembly = "(" + label + ")";
			address = addr;
		} else {
			disassembly = "(" + addr + ")";
			address = "";
		}
		opcodes += fmt::format("{:02x} {:02x}", 
							  wordval & 0xff, (wordval >> 8) & 0xff);
		break;

	case AddressingMode::IndirectX: // ($xx,X)
		byteval = readByteAtPC();
		wordval = byteval + X;
		if (wordval > 0xFF)
			wordval -= 0xFF;
		wordval = readWord(wordval);
		
		label = addressLabelSearch(byteval);
		addr = fmt::format("${:04x}", byteval);
		if (!label.empty())  {
			disassembly = label;
			address = addr;
		} else { 
			disassembly = addr;
			address = "";
		}
		disassembly = "(" + disassembly + "),X";
		opcodes += fmt::format("{:02x}", byteval);
		if (atPC) 
			computedAddr = fmt::format("${:04x}", wordval);
		break;
		
	case AddressingMode::IndirectY:  // ($xx),Y
		byteval = readByteAtPC();
		wordval = readWord(byteval);
		wordval += Y;
		
		label = addressLabel(byteval);
		addr = fmt::format("${:04x}", byteval);
		if (!label.empty()) { 
			disassembly = label;
			address = addr; 
		} else {
			disassembly = addr;
			address = "";
		}
		disassembly = "(" + disassembly + "),Y";
		opcodes += fmt::format("{:02x}", byteval);
		if (atPC) 
			computedAddr = fmt::format("${:04x}", wordval);
		break;
	
	default:
		disassembly += fmt::format("[Invalid addressing mode]");
	}
}

Word MOS6502::disassembleAt(Word dPC, std::string& disassembly) {
	Word savePC = PC;
	Cycles_t saveCycles = _cycles;
	std::string ins, bkpoint, args, opcodes, marker, address, computedAddress;
	bool atPC = (PC == dPC); 
	bool bytes = false;

	PC = dPC;
	if (isBreakpoint(PC))
		bkpoint = "B";

	if (atPC) 
		marker = "*";

	Byte opcode = readByteAtPC();
	opcodes = fmt::format("{:02x} ", opcode);

	if (_instructions.count(opcode) == 0) {
		ins = fmt::format(".byte ${:02x}", opcode);
		bytes = true;
	}
	else {
		ins = _instructions.at(opcode).name;
		decodeArgs(atPC, opcode, args, opcodes, address, computedAddress);
	}

    //  B*| label^addr  : | 23 56 89 | ins     args | [opt. address]
	auto addr = fmt::format("{:04x}", dPC);
	std::string label = addressLabel(dPC);
	if (!label.empty()) 
		 addr += fmt::format(" ({})", label);
		
	disassembly = fmt::format("{:1.1}{:1.1}| {:20.20} | {:9.9}| {:<7}", 
				              marker, bkpoint, addr, opcodes, ins);

	if (!bytes) 
		disassembly += fmt::format("{:<20} | {:<5.5} | {}", args, address, computedAddress);
    dPC = PC;
	PC = savePC;
	_cycles = saveCycles;

	return dPC;
}

Word MOS6502::disassemble(Word dPC, uint64_t cnt) {
	std::string disassembly;

	if (dPC > MAX_MEM) {
		fmt::print("PC at end of memory");
		return dPC;
	}

	do {
		dPC = disassembleAt(dPC, disassembly);
		fmt::print("{}\n", disassembly);
	} while (--cnt && dPC < MAX_MEM);
	
	return dPC;
}

#ifdef TEST_BUILD
// This is used for basic disassembler testing
void MOS6502::traceOneInstruction() {
	disassemble(PC, 1);
	executeOneInstruction();
}
#endif
