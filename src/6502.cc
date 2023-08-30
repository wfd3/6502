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

CPU::CPU(Memory<Address_t, Byte> *m) {
	mem = m;
	CPU::setupInstructionMap();
	overrideResetVector = false;
	pendingReset = true;
	debugEntryFunc = NULL;
	debugExitFunc = NULL;
}

void CPU::setResetVector(Word address) {
	writeWord(RESET_VECTOR, address);
}

void CPU::setInterruptVector(Word address) {
	writeWord(INTERRUPT_VECTOR, address);
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
		PC = readWord(RESET_VECTOR);
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

void CPU::exception(const char *fmt_str, ...) {
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
	debug();
}

bool CPU::isNegative(Byte val) {
	return (val & NegativeBit);
}

void CPU::setFlagN(Byte val) {
	Flags.N = (val & NegativeBit) != 0;
}

void CPU::setFlagZ(Byte val) {
	Flags.Z = (val == 0);
}

Byte CPU::readByte(Word address) {
	Byte data = mem->Read(address);
	Cycles++;
	return data;
}

void CPU::writeByte(Word address, Byte value) {
	mem->Write(address, value);
	Cycles++;
}

Word CPU::readWord(Word address) {
	Word w = readByte(address) | (readByte(address + 1) << 8);
	return w;
}

void CPU::writeWord(Word address, Word word) {
	writeByte(address, word & 0xff);
	writeByte(address + 1, (word >> 8));
}

Word CPU::readWordAtPC() {
	Word w = readByteAtPC() | (readByteAtPC() << 8);
	return w;
}

Byte CPU::readByteAtPC() {
	Byte opcode = readByte(PC);
	PC++;
	return opcode;
}

void CPU::pushWord(Word value) {
	Byte b;

	b = (Byte) (value >> 8); // value high
	push(b);

	b = (Byte) value & 0xff; // value low
	push(b);
}

Word CPU::popWord() {
	Word w;

	// Low byte then high byte
	w = pop() | (pop() << 8);
	return w;
}

void CPU::push(Byte value) {
	Word SPAddress = STACK_FRAME + SP;
	writeByte(SPAddress, value);
	SP--;
}

Byte CPU::pop() {
	Word SPAddress;
	SP++;
	SPAddress = STACK_FRAME + SP;
	return readByte(SPAddress);
}

void CPU::pushPS() {
	// PHP silently sets the Unused flag (bit 5) and the Break
	// flag (bit 4)
	push(PS | UnusedBit | BreakBit);
}

void CPU::popPS() {
	PS = pop();
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
		address = readByteAtPC();
		break;

	// ZeroPage,X 
	case ADDR_MODE_ZPX:
		zpaddr = readByteAtPC() + X;
		address = zpaddr;
		Cycles++;
		break;

        // ZeroPage,Y 
	case ADDR_MODE_ZPY:
		zpaddr = readByteAtPC() + Y;
		address = zpaddr;
		Cycles++;
		break;

	// Relative
	case ADDR_MODE_REL:
		rel = SByte(readByteAtPC());
		address = PC + rel;
		break;

	// Absolute
	case ADDR_MODE_ABS:
		address = readWordAtPC();
		break;

	// Absolute,X 
	case ADDR_MODE_ABX:
		address = readWordAtPC();
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
		address = readWordAtPC();
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
		address = readWordAtPC();
		break;

        // (Indirect,X) or Indexed Indirect
	case ADDR_MODE_IDX:	
		zpaddr = readByteAtPC() + X;
		address = readWord(zpaddr);
		Cycles++;
		break;

	// (Indirect),Y or Indirect Indexed
	case ADDR_MODE_IDY:
		address = readByteAtPC();
		address = readWord(address);
		address += Y;
		break;

	default:
		exception("Invalid addressing mode: 0x%ld\n",
			  instructions[opcode].addrmode);
		break;
	}
	
	return address;
}

Byte CPU::getData(Byte opcode, Byte &expectedCycles) {
	Byte data;
	Word address;

	switch (instructions[opcode].addrmode) {

	// Implied and Accumulator
	case ADDR_MODE_IMP:
	case ADDR_MODE_ACC:
		data = 0;
		break;

	// Immediate mode
	case ADDR_MODE_IMM:
		data = readByteAtPC();
		break;

	default:
		address = getAddress(opcode, expectedCycles);
		data = readByte(address);
		break;
	}

	return data;
}

std::tuple<Byte, Byte> CPU::executeOneInstruction() {
	Byte opcode;
	Cycles_t startCycles;
	Byte expectedCyclesToUse;
	Word startPC;
	opfn_t op;

	startPC = PC;
	startCycles = Cycles;

	opcode = readByteAtPC();
	if (instructions.count(opcode) == 0) {
		PC--;
		exception("Invalid opcode 0x%x at PC 0x%04x\n", opcode, PC);
	}

	expectedCyclesToUse = instructions[opcode].cycles;
	op = instructions[opcode].opfn;

	(this->*op)(opcode, expectedCyclesToUse);

	if (debug_loopDetection && startPC == PC) {
		printf("# Loop detected at %04x, entering debugger\n", PC);
		debugMode = true;
	}

	if (pendingReset) {
		exitReset();
		startCycles = Cycles; // startCycles is wrong, reset it.
	}
	
	return std::make_tuple(Cycles - startCycles, expectedCyclesToUse);
}

void CPU::execute() {
	while (1) {
		if (CPU::debugMode || CPU::isBreakpoint(PC))
			CPU::debug();
		else if (CPU::isPCAtExitAddress())
			break;
		else
			CPU::executeOneInstruction();
	}
}

