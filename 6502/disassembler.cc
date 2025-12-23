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

// Disassembly output formatting constants
namespace {
	constexpr int MARKER_WIDTH = 1;
	constexpr int BREAKPOINT_WIDTH = 1;
	constexpr int ADDRESS_WIDTH = 20;
	constexpr int OPCODES_WIDTH = 9;
	constexpr int INSNAME_WIDTH = 7;
	constexpr int ARGS_WIDTH = 20;
	constexpr int OPTIONAL_ADDR_WIDTH = 5;
}

// Helper function to format an address with optional label
void MOS6502::formatAddressWithLabel(Word value, const std::string& formatStr,
                                     std::string& disasm, std::string& addr,
                                     bool useSearch) {
	std::string label = useSearch ?
		debugger.addressLabelSearch(value) :
		debugger.addressLabel(value);
	addr = fmt::vformat(formatStr, fmt::make_format_args(value));
	if (!label.empty()) {
		disasm = label;
	} else {
		disasm = addr;
		addr = "";
	}
}

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
		formatAddressWithLabel(byteval, "${:02x}", disassembly, address, true);
		opcodes += fmt::format("{:02x} ", byteval);
		break;

	case AddressingMode::ZeroPageX:  // $xx,X
		byteval = readByte(dPC++);
		formatAddressWithLabel(byteval, "${:04x}", disassembly, address, true);
		disassembly += ",X";
		opcodes += fmt::format("{:02x} ", byteval);
		if (atPC)
			computedAddr = fmt::format("${:04x}", byteval + _ctx.X);
		break;

	case AddressingMode::ZeroPageY:  // $xx,Y
		byteval = readByte(dPC++);
		formatAddressWithLabel(byteval, "${:04x}", disassembly, address, true);
		disassembly += ",Y";
		opcodes += fmt::format("{:02x} ", byteval);
		if (atPC)
			computedAddr = fmt::format("${:04x}", byteval + _ctx.Y);
		break;

	case AddressingMode::Relative:
		rel = readByte(dPC++);
		wordval = dPC + SByte(rel);
		formatAddressWithLabel(wordval, "${:04x}", disassembly, address, false);
		// If no label found, show relative offset instead
		if (address.empty())
			disassembly = fmt::format("#${:02x}", rel);
		else
			address = fmt::format("${:04x}", wordval);
		opcodes += fmt::format("{:02x} ", rel);
		break;

	case AddressingMode::Absolute:  // $xxxx
		wordval = readWord(dPC);
		dPC += 2;
		formatAddressWithLabel(wordval, "${:04x}", disassembly, address, false);
		opcodes += fmt::format("{:02x} {:02x}", wordval & 0xff, (wordval >> 8) & 0xff);
		break;

	case AddressingMode::AbsoluteX:  // $xxxx,X
		wordval = readWord(dPC);
		dPC += 2;
		formatAddressWithLabel(wordval, "${:04x}", disassembly, address, true);
		disassembly += ",X";
		opcodes += fmt::format("{:02x} {:02x}", wordval & 0xff, (wordval >> 8) & 0xff);
		if (atPC)
			computedAddr = fmt::format("${:04x}", wordval + _ctx.X);
		break;		

	case AddressingMode::AbsoluteY:  // $xxxx,Y
		wordval = readWord(dPC);
		dPC += 2;
		formatAddressWithLabel(wordval, "${:04x}", disassembly, address, true);
		disassembly += ",Y";
		opcodes += fmt::format("{:02x} {:02x}", wordval & 0xff, (wordval >> 8) & 0xff);
		if (atPC)
			computedAddr = fmt::format("${:04x}", wordval + _ctx.Y);
		break;
		
	case AddressingMode::Indirect:  // $(xxxx)
		wordval = readWord(dPC);
		dPC += 2;
		formatAddressWithLabel(wordval, "${:04x}", disassembly, address, false);
		disassembly = "(" + disassembly + ")";
		opcodes += fmt::format("{:02x} {:02x}", wordval & 0xff, (wordval >> 8) & 0xff);
		break;

	case AddressingMode::IndirectX: // ($xx,X)
		byteval = readByte(dPC++);
		formatAddressWithLabel(byteval, "${:04x}", disassembly, address, true);
		disassembly = "(" + disassembly + "),X";
		opcodes += fmt::format("{:02x}", byteval);
		if (atPC) {
			wordval = byteval + _ctx.X;
			if (wordval > 0xFF)
				wordval &= 0xFF;
			wordval = readWord(wordval);
			computedAddr = fmt::format("${:04x}", wordval);
		}
		break;
		
	case AddressingMode::IndirectY:  // ($xx),Y
		byteval = readByte(dPC++);
		formatAddressWithLabel(byteval, "${:04x}", disassembly, address, false);
		disassembly = "(" + disassembly + "),Y";
		opcodes += fmt::format("{:02x}", byteval);
		if (atPC) {
			wordval = readWord(byteval);
			wordval += _ctx.Y;
			computedAddr = fmt::format("${:04x}", wordval);
		}
		break;
	
	default:
		disassembly = fmt::format("[Invalid addressing mode]");
	}
}

Word MOS6502::disassembleAt(Word dPC, std::string& disassembly) {
	std::string insname, brkpoint, args, opcodes, marker, address, computedAddress;
	struct instruction ins;
	const bool atPC = (_ctx.PC == dPC);
	Word _pc = dPC; 

	if (debugger.isBreakpoint(dPC))
		brkpoint = "B";

	if (atPC) 
		marker = "*";

	Byte opcode = readByte(_pc++);
	opcodes = fmt::format("{:02x} ", opcode);

	auto validOpcode = decodeInstruction(opcode, ins);
	if (validOpcode) {
		insname = ins.name;
		decodeArgs(_pc, atPC, opcode, args, opcodes, address, computedAddress);
	} else {
		insname = fmt::format(".byte ${:02x}", opcode);
	}

    //  B*| label^addr  : | 23 56 89 | ins     args | [opt. address] | [computed indexed address (if registers are valid)]
	auto addr = fmt::format("{:04x}", dPC);
	std::string label = debugger.addressLabel(dPC);
	if (!label.empty())
		 addr += fmt::format(" ({})", label);

	disassembly = fmt::format("{:{}}{:{}}| {:{}}.{} | {:{}}.{}| {:<{}}",
		marker, MARKER_WIDTH,
		brkpoint, BREAKPOINT_WIDTH,
		addr, ADDRESS_WIDTH, ADDRESS_WIDTH,
		opcodes, OPCODES_WIDTH, OPCODES_WIDTH,
		insname, INSNAME_WIDTH);

	if (validOpcode)
		disassembly += fmt::format("{:<{}} | {:<{}}.{} | {}",
			args, ARGS_WIDTH,
			address, OPTIONAL_ADDR_WIDTH, OPTIONAL_ADDR_WIDTH,
			computedAddress);

	_cycles = 0;

	return _pc;
}

Word MOS6502::disassemble(Word dPC, uint64_t cnt) {
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
	disassemble(_ctx.PC, 1);
	executeOneInstruction();
}
#endif
