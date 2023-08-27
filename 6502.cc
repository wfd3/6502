//
// Core emulated 6502 CPU functions
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
#include <cstring>
#include <memory>
#include <cstdarg>

#include <unistd.h>

#include "6502.h"

CPU::CPU(Memory *m) {
	mem = m;
	CPU::setupInstructionMap();
	overrideResetVector = false;
	pendingReset = true;
}

void CPU::setResetVector(Word address) {
	WriteWord(RESET_VECTOR, address);
}

void CPU::setInterruptVector(Word address) {
	WriteWord(INTERRUPT_VECTOR, address);
}

void CPU::exitReset() {
	SP = INITIAL_SP;
	A = X = Y = 0;
	PS = 0; //C = Z = I = D = B = V = N = 0;

	debugMode = false;
	debug_alwaysShowPS = false;
	debug_lastCmd = "";
	_exitAddressSet = false;
	if (overrideResetVector) {
		PC = pendingResetPC;
	} else {
		PC = ReadWord(RESET_VECTOR);
	}
	pendingReset = false;
	overrideResetVector = false;

	// Do this last incase anything above ever changes Cycles by
	// side-effect.
	Cycles = 7;		
}	

// This is only intended for testing, not for emulation.
// TODO: Change this function name and update all the 6502 tests.
void CPU::Reset(Word address) {
	// TODO: these should be lock-protected.
	pendingReset = true;
	overrideResetVector = true;
	pendingResetPC = address;
	exitReset();
}

void CPU::Reset() {
	// TODO: This should be atomic.
	pendingReset = true;
}

void CPU::Exception(const char *fmt_str, ...) {
	va_list args;
	int final_n, n = 256;
	std::unique_ptr<char[]> formatted;

	va_start(args, fmt_str);
	while (1) {
		// Wrap the plain char array into the unique_ptr 
		formatted.reset(new char[n]); 
		strcpy(&formatted[0], fmt_str);

		final_n = vsnprintf(formatted.get(), n, fmt_str, args);

		if (final_n < 0 || final_n >= n)
			n += abs(final_n - n + 1);
		else
			break;
	}

	va_end(args);

	printf("CPU Exception: %s\n", formatted.get());
	printf("Entering debugger\n");
	debugMode = true;
	Debug();
}

bool CPU::isNegative(Byte val) {
	return (val & NegativeBit);
}

void CPU::SetFlagN(Byte val) {
	Flags.N = (val & NegativeBit) != 0;
}

void CPU::SetFlagZ(Byte val) {
	Flags.Z = (val == 0);
}

Byte CPU::ReadByte(Word address) {
	Byte data = mem->Read(address);
	Cycles++;
	return data;
}

void CPU::WriteByte(Word address, Byte value) {
	mem->Write(address, value);
	Cycles++;
}

Word CPU::ReadWord(Word address) {
	Word w = ReadByte(address) | (ReadByte(address + 1) << 8);
	return w;
}

void CPU::WriteWord(Word address, Word word) {
	WriteByte(address, word & 0xff);
	WriteByte(address + 1, (word >> 8));
}

Word CPU::ReadWordAtPC() {
	Word w = ReadByteAtPC() | (ReadByteAtPC() << 8);
	return w;
}

Byte CPU::ReadByteAtPC() {
	Byte opcode = ReadByte(PC);
	PC++;
	return opcode;
}

void CPU::PushWord(Word value) {
	Byte b;

	b = (Byte) (value >> 8); // value high
	Push(b);

	b = (Byte) value & 0xff; // value low
	Push(b);
}

Word CPU::PopWord() {
	Word w;

	// Low byte then high byte
	w = Pop() | (Pop() << 8);
	return w;
}

void CPU::Push(Byte value) {
	Word SPAddress = STACK_FRAME + SP;
	WriteByte(SPAddress, value);
	SP--;
}

Byte CPU::Pop() {
	Word SPAddress;
	SP++;
	SPAddress = STACK_FRAME + SP;
	return ReadByte(SPAddress);
}

void CPU::PushPS() {
	// PHP silently sets the Unused flag (bit 5) and the Break
	// flag (bit 4)
	Push(PS | UnusedBit | BreakBit);
}

void CPU::PopPS() {
	PS = Pop();
	Flags.B = false;
	Flags._unused = false;
}

Word CPU::getAddress(Byte opcode, Byte &expectedCycles) {
	Word address;
	Byte zpaddr;
	Byte flags;
	SByte rel;

	flags = instructions[opcode].flags;
	
	switch (instructions[opcode].addrmode) {

        // ZeroPage mode
	case ADDR_MODE_ZP:
		address = ReadByteAtPC();
		break;

	// ZeroPage,X 
	case ADDR_MODE_ZPX:
		zpaddr = ReadByteAtPC() + X;
		address = zpaddr;
		Cycles++;
		break;

        // ZeroPage,Y 
	case ADDR_MODE_ZPY:
		zpaddr = ReadByteAtPC() + Y;
		address = zpaddr;
		Cycles++;
		break;

	// Relative
	case ADDR_MODE_REL:
		rel = SByte(ReadByteAtPC());
		address = PC + rel;
		break;

	// Absolute
	case ADDR_MODE_ABS:
		address = ReadWordAtPC();
		break;

	// Absolute,X 
	case ADDR_MODE_ABX:
		address = ReadWordAtPC();
		// Add a cycle if a page boundry is crossed
		if ((flags == CYCLE_CROSS_PAGE) && ((address + X) >> 8) !=
		    (address >> 8)) {
			expectedCycles++;
			Cycles++;
		}
		address += X;
		break;

	// Absolute,Y 
	case ADDR_MODE_ABY:
		address = ReadWordAtPC();
		// Add a cycle if a page boundry is crossed
		if ((flags == CYCLE_CROSS_PAGE) && ((address + Y) >> 8) !=
		    (address >> 8)) {
			expectedCycles++;
			Cycles++;
		}
		address += Y;
		break;

	// Indirect 
	case ADDR_MODE_IND:
		address = ReadWordAtPC();
		break;

        // (Indirect,X) or Indexed Indirect
	case ADDR_MODE_IDX:	
		zpaddr = ReadByteAtPC() + X;
		address = ReadWord(zpaddr);
		Cycles++;
		break;

	// (Indirect),Y or Indirect Indexed
	case ADDR_MODE_IDY:
		address = ReadByteAtPC();
		address = ReadWord(address);
		address += Y;
		break;

	default:
		Exception("Invalid addressing mode: 0x%ld\n",
			  instructions[opcode].addrmode);
		break;
	}
	
	return address;
}

Byte CPU::getData(Byte opcode, Byte &expectedCycles) {
	Byte data;
	Word address;

	switch (instructions[opcode].addrmode) {
	case ADDR_MODE_IMP:
	case ADDR_MODE_ACC:
		return 0;

	// Immediate mode
	case ADDR_MODE_IMM:
		data = ReadByteAtPC();
		break;

	default:
		address = getAddress(opcode, expectedCycles);
		data = ReadByte(address);
		break;
	}

	return data;
}

bool CPU::isPCAtExitAddress() {
	return _exitAddressSet && (PC == _exitAddress);
}

void CPU::setExitAddress(Address_t _pc) {
	_exitAddress = _pc;
	_exitAddressSet = true;
}

void CPU::unsetExitAddress() {
	_exitAddressSet = false;
}

std::tuple<Byte, Byte> CPU::ExecuteOneInstruction() {
	Byte opcode;
	Cycles_t startCycles;
	Byte expectedCyclesToUse;
	Word startPC;
	opfn_t op;

	startPC = PC;
	startCycles = Cycles;

	opcode = ReadByteAtPC();
	if (instructions.count(opcode) == 0) {
		PC--;
		Exception("Invalid opcode 0x%x at PC 0x%04x\n", opcode, PC);
	}

	expectedCyclesToUse = instructions[opcode].cycles;
	op = instructions[opcode].opfn;

	(this->*op)(opcode, expectedCyclesToUse);

	if (debug_loopDetection && startPC == PC) {
		printf("# Loop detected, forcing break at %04x\n", PC);
		//CPU::addBreakpoint(PC);
		debugMode = true;
	}

	// If the CPU was reset, startCycles could be wrong.  Check and reset.
	if (startCycles > Cycles)
		startCycles = Cycles;

	if (pendingReset)
		exitReset();
	
	return std::make_tuple(Cycles - startCycles, expectedCyclesToUse);
}

void CPU::Execute() {

	while (1) {
		if (CPU::debugMode || CPU::isBreakpoint(PC))
			CPU::Debug();
		else if (CPU::isPCAtExitAddress())
			return;
		else
			CPU::ExecuteOneInstruction();
	}
}

