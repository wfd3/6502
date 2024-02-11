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

// Don't use read{Byte,Word}AtPC() in the disassembler, as that increments the Program Counter.  

void MOS6502::decodeArgs(Word& dPC, const bool atPC, const Byte opcode, std::string& disassembly, 
					     std::string& opcodes, std::string& address, std::string& computedAddr) {
	auto mode = getInstructionAddressingMode(opcode);
	Byte byteval;
	Word wordval;
	Byte rel;
	std::string out, addr, label;

	// Note: if atPC is true, then the registers are valid to compute absolute indexed or Zero Page indexed addresses.

	switch (mode) {
	case AddressingMode::Implied:
		break;

	case AddressingMode::Accumulator:
		disassembly = "A";
		address = "";
		break;

	case AddressingMode::Immediate:  // #$xx
		byteval = readByte(dPC++);
		disassembly = fmt::format("#${:02x}", byteval);
		opcodes += fmt::format("{:02x} ", byteval);
		address = "";
		break;

	case AddressingMode::ZeroPage:  // $xx
		byteval = readByte(dPC++);
		label = debugger.addressLabelSearch(byteval);
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
		byteval = readByte(dPC++);
		label = debugger.addressLabelSearch(byteval);
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
		byteval = readByte(dPC++);
		label = debugger.addressLabelSearch(byteval);
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
		rel = readByte(dPC++);
		wordval = dPC + SByte(rel);
		addr = fmt::format("${:04x}", wordval);
		label = debugger.addressLabel(wordval);

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
		wordval = readWord(dPC);
		dPC += 2;
		label = debugger.addressLabel(wordval);
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
		wordval = readWord(dPC);
		dPC += 2;
		label = debugger.addressLabelSearch(wordval);
		addr = fmt::format("${:04x}", wordval);
		if (!label.empty())  {
			disassembly = label;
			address = addr;
		} else { 
			disassembly = addr;
			address = "";
		}
		disassembly += ",X";
		opcodes += fmt::format("{:02x} {:02x}", wordval & 0xff, (wordval >> 8) & 0xff);
		if (atPC) 
			computedAddr = fmt::format("${:04x}", wordval + X);
	
		break;		

	case AddressingMode::AbsoluteY:  // $xxxx,Y
		fmt::print("ABS,Y\n");
		wordval = readWord(dPC);
		dPC += 2;
		label = debugger.addressLabelSearch(wordval);
		addr = fmt::format("${:04x}", wordval);
		if (!label.empty()) {
			disassembly = label;
			address = addr;
		} else {
			disassembly = addr;
			address = "";
		}
		disassembly += ",Y";
		opcodes += fmt::format("{:02x} {:02x}", wordval & 0xff, (wordval >> 8) & 0xff);
		if (atPC) 
			computedAddr = fmt::format("${:04x}", wordval + Y);
		break;
		
	case AddressingMode::Indirect:  // $(xxxx)
		wordval = readWord(dPC);
		dPC += 2;

		label = debugger.addressLabel(wordval);
		addr = fmt::format("{:04x}", wordval);
		if (!label.empty()) {
			disassembly = "(" + label + ")";
			address = addr;
		} else {
			disassembly = "(" + addr + ")";
			address = "";
		}
		opcodes += fmt::format("{:02x} {:02x}", wordval & 0xff, (wordval >> 8) & 0xff);
		break;

	case AddressingMode::IndirectX: // ($xx,X)
		byteval = readByte(dPC++);

		label = debugger.addressLabelSearch(byteval);
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
		if (atPC) {
			wordval = byteval + X;
			if (wordval > 0xFF)
				wordval -= 0xFF;
			wordval = readWord(wordval);
		
			computedAddr = fmt::format("${:04x}", wordval);
		}
		break;
		
	case AddressingMode::IndirectY:  // ($xx),Y
		byteval = readByte(dPC++);
		
		label = debugger.addressLabel(byteval);
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
		if (atPC) {
			wordval = readWord(byteval);
			wordval += Y;
			computedAddr = fmt::format("${:04x}", wordval);
		}
		break;
	
	default:
		disassembly += fmt::format("[Invalid addressing mode]");
	}
}

Word MOS6502::disassembleAt(Word dPC, std::string& disassembly) {
	std::string insname, brkpoint, args, opcodes, marker, address, computedAddress;
	struct instruction ins;
	const bool atPC = (PC == dPC); 

	if (debugger.isBreakpoint(dPC))
		brkpoint = "B";

	if (atPC) 
		marker = "*";

	Byte opcode = readByte(dPC++);
	opcodes = fmt::format("{:02x} ", opcode);

	auto validOpcode = decodeInstruction(opcode, ins);
	if (validOpcode) {
		insname = ins.name;
		decodeArgs(dPC, atPC, opcode, args, opcodes, address, computedAddress);
	} else {
		insname = fmt::format(".byte ${:02x}", opcode);
	}

    //  B*| label^addr  : | 23 56 89 | ins     args | [opt. address] | [computed indexed address (if registers are valid)]
	auto addr = fmt::format("{:04x}", dPC);
	std::string label = debugger.addressLabel(dPC);
	if (!label.empty()) 
		 addr += fmt::format(" ({})", label);
		
	disassembly = fmt::format("{:1.1}{:1.1}| {:20.20} | {:9.9}| {:<7}", marker, brkpoint, addr, opcodes, insname);

	if (validOpcode) 
		disassembly += fmt::format("{:<20} | {:<5.5} | {}", args, address, computedAddress);

	_cycles = 0;

	return dPC;
}

Word MOS6502::disassemble(Word dPC, uint64_t cnt = 1) {
	std::string disassembly;

	if (dPC > LAST_ADDRESS) {
		fmt::print("PC at end of memory");
		return dPC;
	}

	do {
		dPC = disassembleAt(dPC, disassembly);
		fmt::print("{}\n", disassembly);
	} while (--cnt && dPC < LAST_ADDRESS);
	
	return dPC;
}

#ifdef TEST_BUILD
// This is used for basic disassembler testing
void MOS6502::traceOneInstruction() {
	disassemble(PC, 1);
	executeOneInstruction();
}
#endif
