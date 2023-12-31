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

#include "6502.h"

//////////
// CPU Setup and reset
MOS6502::MOS6502(Memory<Address_t, Byte>& m) : mem(m),
				       _instructions(setupInstructionMap()),
				       _debugCommands(setupDebugCommands()) {
	
	_inReset = true;
	initDebugger();
}

void MOS6502::setResetVector(Word address) {
	writeWord(RESET_VECTOR, address);
}

void MOS6502::setInterruptVector(Word address) {
	writeWord(INTERRUPT_VECTOR, address);
}

void MOS6502::exitReset() {
	PC = readWord(RESET_VECTOR);
	SP = INITIAL_SP;

#ifdef TEST_BUILD
	// If we're here via TestReset() clobber the PC and SP with test values
	if (_testReset) {
		SP = testResetSP;
		PC = testResetPC;
	}
	_testReset = false;
#endif

	_debuggingEnabled = false;
	debug_alwaysShowPS = false;

	_hitException = false;

	_inReset = false;
	_pendingReset = false;

	Cycles += 7;		
}	

// This is only intended for testing, not for emulation.  It
// allows tests to set specific starting Program Counter and
// Stack Pointer values, and arranges for the next call to execute...() 
// to exit the CPU from reset.
#ifdef TEST_BUILD
void MOS6502::TestReset(Word initialPC, Byte initialSP)  {
	_inReset = false;
	_pendingReset = true;
	_testReset = true;
	testResetPC = initialPC;
	testResetSP = initialSP;
}
#endif

// 'Asserts' the Reset line if not asserted, de-asserts the Reset line
// if asserted. 
void MOS6502::Reset() {
	if (!_inReset) {		// Not in Reset, assert the Reset line
		_inReset = true;
	} else {				// In Reset, de-assert Reset
		_inReset = false;
		_pendingReset = true;
	}
}

//////////
// Interrupts

void MOS6502::interrupt() {

	pushWord(PC);
	pushPS();

	Flags.I = 1;
	PC = readWord(INTERRUPT_VECTOR);
	Cycles++;
}

bool MOS6502::NMI() {
	if (_pendingNMI) {
		addBacktraceInterrupt(PC);
		_IRQCount++;
		interrupt();
		_pendingNMI = false;
		return true;
	}

	return false;
}

bool MOS6502::IRQ() {
	if (_pendingIRQ && !IRQBlocked()) {
		addBacktraceInterrupt(PC);
		_NMICount++;
		interrupt();
		_pendingIRQ = false;
		return true;
	}

	return false;
}

//////////
// CPU Exception
void MOS6502::exception(const std::string &message) {
	std::string msg = "6502 CPU Exception: " + message;
	_hitException = true;
	throw std::runtime_error(msg);
}

//////////
// Flags
bool MOS6502::isNegative(Byte val) {
	return (val & NegativeBit);
}

void MOS6502::setFlagNByValue(Byte val) {
	Flags.N = isNegative(val);
}

void MOS6502::setFlagZByValue(Byte val) {
	Flags.Z = (val == 0);
}

bool MOS6502::IRQBlocked() {
	return Flags.I == 1;
}

//////////
// Memory access
Byte MOS6502::readByte(Word address) {
	Byte data = mem.Read(address);
	Cycles++;
	return data;
}

void MOS6502::writeByte(Word address, Byte value) {
	mem.Write(address, value);
	Cycles++;
}

Word MOS6502::readWord(Word address) {
	Word w = readByte(address) | (readByte(address + 1) << 8);
	return w;
}

void MOS6502::writeWord(Word address, Word word) {
	writeByte(address, word & 0xff);
	writeByte(address + 1, (Byte) (word >> 8));
}

Word MOS6502::readWordAtPC() {
	Word w = readByteAtPC() | (readByteAtPC() << 8);
	return w;
}

Byte MOS6502::readByteAtPC() {
	Byte opcode = readByte(PC);
	PC++;
	return opcode;
}

//////////
// Stack operations
void MOS6502::push(Byte value) {
	Word SPAddress = STACK_FRAME + SP;
	writeByte(SPAddress, value);
	SP--;
}

Byte MOS6502::pop() {
	Word SPAddress;
	SP++;
	SPAddress = STACK_FRAME + SP;
	return readByte(SPAddress);
}

void MOS6502::pushWord(Word value) {
	push((Byte) (value >> 8)); // value high
	push((Byte) value & 0xff); // value low
}

Word MOS6502::popWord() {
	// Low byte then high byte
	Word w = pop() | (pop() << 8);
	return w;
}

void MOS6502::pushPS() {
	// PHP silently sets the Unused flag (bit 5) and the Break
	// flag (bit 4)
	push(PS | UnusedBit | BreakBit);
}

void MOS6502::popPS() {
	PS = pop();
	Flags.B = false;
	Flags._unused = false;
}

//////////
// Address decoding
Word MOS6502::getAddress(Byte opcode, Cycles_t &expectedCycles) {
	Word address;
	SByte rel;

	// Add a cycle if a page boundary is crossed
	auto updateCycles = [&](Byte reg) {
		if (( _instructions.at(opcode).flags == InstructionFlags::PageBoundary) &&
		    ((address + reg) >> 8) != (address >> 8)) {
			expectedCycles++;
			Cycles++;
		}
	};

	auto addressMode = _instructions.at(opcode).addrmode;
	
	switch(addressMode) {

    // ZeroPage mode
	case AddressingMode::ZeroPage:
		address = readByteAtPC();
		break;

	// ZeroPage,X (with zero page wrap around)
	case AddressingMode::ZeroPageX:
		address = static_cast<Byte>(readByteAtPC() + X);
		Cycles++;
		break;

	// ZeroPage,Y (with zero page wrap around)
	case AddressingMode::ZeroPageY:
		address = static_cast<Byte>(readByteAtPC() + Y);
		Cycles++;
		break;

	// Relative
	case AddressingMode::Relative:
		rel = SByte(readByteAtPC());
		address = (Word) (PC + rel);
		break;

	// Absolute
	case AddressingMode::Absolute:
		address = readWordAtPC();
		break;

	// Absolute,X 
	case AddressingMode::AbsoluteX:
		address = readWordAtPC();
		updateCycles(X);
		address += X;
		break;

	// Absolute,Y 
	case AddressingMode::AbsoluteY:
		address = readWordAtPC();
		updateCycles(Y);
		address += Y;
		break;

    // (Indirect,X) or Indexed Indirect (with zero page wrap around)
	case AddressingMode::IndirectX:	
		address = static_cast<Byte>(readByteAtPC() + X);
		address = readWord(address);
		Cycles++;
		break;

	// (Indirect),Y or Indirect Indexed
	case AddressingMode::IndirectY:
		address = readByteAtPC();
		address = readWord(address) + Y;
		break;

	case AddressingMode::Implied:
	case AddressingMode::Accumulator:
	case AddressingMode::Immediate:
	case AddressingMode::Indirect:
		exception("Decoded address for Accumulator, Immediate or Indirect opcode");
		break;

	default:
		exception("Invalid addressing mode");
		break;
	}
	
	return address;
}

Byte MOS6502::getData(Byte opcode, Cycles_t &expectedCycles) {
	Byte data = 0;
	Word address;

	auto addressMode = _instructions.at(opcode).addrmode;
	
	switch(addressMode) {

	// Implied and Accumulator
	case AddressingMode::Implied:
	case AddressingMode::Accumulator:
		exception("Tried to fetch address or data for Implicit or Accumulator addressing mode");
		break;

	// Immediate mode
	case AddressingMode::Immediate:
		data = readByteAtPC();
		break;

	default:
		address = getAddress(opcode, expectedCycles);
		data = readByte(address);
		break;
	}

	return data;
}

//////////
// Instruction execution
void MOS6502::executeOneInstructionWithCycleCount(Cycles_t& usedCycles, Cycles_t& expectedCyclesToUse) {
	Byte opcode;
	Word startPC;
	opfn_t op;

	if (_inReset)
		return;

	if (_pendingReset) {
		exitReset();
	}
	
	startPC = PC;
	Cycles = 0;

	opcode = readByteAtPC();
	if (_instructions.count(opcode) == 0) {
		PC--;
		auto s = fmt::format("Invalid opcode {:04x} at PC {:04x}", opcode, PC);
		exception(s);
		return;
	}

	expectedCyclesToUse = _instructions.at(opcode).cycles;
	op = _instructions.at(opcode).opfn;

	(this->*op)(opcode, expectedCyclesToUse);
	usedCycles = Cycles;

	if (debug_loopDetection && startPC == PC) {
		fmt::print("# Loop detected at {:04x}, entering debugger\n",
			   PC);
		_debuggingEnabled = true;
	}

	// Check for a pending Non-maskable interrupt.  If none then
	// check for a pending interrupt request.  
	if (!NMI())
		IRQ();
}

void MOS6502::execute(bool& atHaltAddress, bool& startDebugOnNextInstruction, Cycles_t& cyclesUsed) {
	atHaltAddress = false;
	startDebugOnNextInstruction = false;
	cyclesUsed = 0;

	if (isPCBreakpoint()) {
		startDebugOnNextInstruction = true;
		return;
	} 

	if (isPCAtHaltAddress()) {
		//fmt::print("At halt address {:04x}\n", PC);
		atHaltAddress = true;
		return;
	}

	Cycles_t expected;
	try {
		executeOneInstructionWithCycleCount(cyclesUsed, expected);
	}
	catch(std::exception &e) {
		if (debug_OnException) {
			fmt::print("{}\n", e.what());
			startDebugOnNextInstruction = true;
		}
	}
}
