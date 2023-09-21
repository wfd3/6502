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
CPU::CPU(Memory<Address_t, Byte>& m) : mem(m),
				       _instructions(setupInstructionMap()),
				       _debugCommands(setupDebugCommands()) {
	
	Cycles.disableTimingEmulation();
	_inReset = true;
}

void CPU::setResetVector(Word address) {
	writeWord(RESET_VECTOR, address);
}

void CPU::setInterruptVector(Word address) {
	writeWord(INTERRUPT_VECTOR, address);
}

void CPU::exitReset() {
	SP = pendingResetSP;
	PC = pendingResetPC;

	debugMode = false;
	debug_alwaysShowPS = false;

	_hitException = false;

	_inReset = false;
	_pendingReset = false;

	Cycles += 7;		
}	

// This is only intended for testing, not for emulation.
// TODO: Change this function name and update all the 6502 tests.
void CPU::Reset(Word initialPC, Byte initialSP)  {
	if (!_inReset) {
		_inReset = true;
	} else {
		_inReset = false;
		_pendingReset = true;
		pendingResetPC = initialPC;
		pendingResetSP = initialSP;
	} 
}

void CPU::Reset() {
	Address_t resetVector = readWord(RESET_VECTOR);
	Reset(resetVector, INITIAL_SP);
}

//////////
// Interrupts

void CPU::interrupt() {

	pushWord(PC);
	pushPS();

	Flags.I = 1;
	PC = readWord(INTERRUPT_VECTOR);
	Cycles++;
}

bool CPU::NMI() {
	if (_pendingNMI.load()) {
		addBacktraceInterrupt(PC);
		_IRQCount++;
		interrupt();
		_pendingNMI = false;
		return true;
	}

	return false;
}

bool CPU::IRQ() {
	if (_pendingIRQ && Flags.I == 0) {
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
void CPU::exception(const std::string &message) {
	std::string msg = "6502 CPU Exception: " + message;
	_hitException = true;
	if (debug_OnException) {
		fmt::print("{}\n", message);
		fmt::print("Entering debugger\n");
		debugMode = true;
		debug();
	} else {
		throw std::runtime_error(msg); 
	}
}

//////////
// Flags
bool CPU::isNegative(Byte val) {
	return (val & NegativeBit);
}

void CPU::setFlagN(Byte val) {
	Flags.N = (val & NegativeBit) != 0;
}

void CPU::setFlagZ(Byte val) {
	Flags.Z = (val == 0);
}

//////////
// Memory access
Byte CPU::readByte(Word address) {
	Byte data = mem.Read(address);
	Cycles++;
	return data;
}

void CPU::writeByte(Word address, Byte value) {
	mem.Write(address, value);
	Cycles++;
}

Word CPU::readWord(Word address) {
	Word w = readByte(address) | (readByte(address + 1) << 8);
	return w;
}

void CPU::writeWord(Word address, Word word) {
	writeByte(address, word & 0xff);
	writeByte(address + 1, (Byte) (word >> 8));
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

//////////
// Stack operations
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

void CPU::pushWord(Word value) {
	push((Byte) (value >> 8)); // value high
	push((Byte) value & 0xff); // value low
}

Word CPU::popWord() {
	// Low byte then high byte
	Word w = pop() | (pop() << 8);
	return w;
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

//////////
// Address decoding
Word CPU::getAddress(Byte opcode, uint64_t &expectedCycles) {
	Word address;
	Byte zpaddr;
	Byte flags;
	SByte rel;

	auto updateCycles = [&](Byte reg) {
		if ((flags == CYCLE_PAGE) &&
		    ((address + reg) >> 8) != (address >> 8)) {
			expectedCycles++;
			Cycles++;
		}
	};

	flags = _instructions.at(opcode).flags;
	
	switch (_instructions.at(opcode).addrmode) {

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
		address = (Word) (PC + rel);
		break;

	// Absolute
	case ADDR_MODE_ABS:
		address = readWordAtPC();
		break;

	// Absolute,X 
	case ADDR_MODE_ABX:
		address = readWordAtPC();
		// Add a cycle if a page boundary is crossed
		updateCycles(X);
		address += X;
		break;

	// Absolute,Y 
	case ADDR_MODE_ABY:
		address = readWordAtPC();
		// Add a cycle if a page boundary is crossed
		updateCycles(Y);
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
		auto s = fmt::format("Invalid addressing mode: {:#04x}",
				     _instructions.at(opcode).addrmode);
		exception(s);
		break;
	}
	
	return address;
}

Byte CPU::getData(Byte opcode, uint64_t &expectedCycles) {
	Byte data;
	Word address;

	switch (_instructions.at(opcode).addrmode) {

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

//////////
// Instruction execution
std::tuple<uint64_t, uint64_t> CPU::executeOneInstruction() {
	Byte opcode;
	Cycles_t startCycles;
	uint64_t expectedCyclesToUse;
	Word startPC;
	opfn_t op;

	if (_inReset)
		return std::make_tuple(0, 0);

	if (_pendingReset) {
		exitReset();
	}
	
	startPC = PC;
	startCycles = Cycles;

	opcode = readByteAtPC();
	if (_instructions.count(opcode) == 0) {
		PC--;
		auto s = fmt::format("Invalid opcode {:04x} at PC {:#04x}",
				     opcode, PC);
		exception(s);
		return std::make_tuple(0, 0);
	}

	expectedCyclesToUse = _instructions.at(opcode).cycles;
	op = _instructions.at(opcode).opfn;

	(this->*op)(opcode, expectedCyclesToUse);
	auto usedCycles = Cycles - startCycles;

	if (debug_loopDetection && startPC == PC) {
		fmt::print("# Loop detected at {:04x}, entering debugger\n",
			   PC);
		debugMode = true;
	}

	// Check for a pending Non-maskable interrupt.  If none then
	// check for a pending interrupt request.  
	if (!NMI())
		IRQ();

	return std::make_tuple(usedCycles, expectedCyclesToUse);
}

bool CPU::executeOne() {
	if (debugMode || isBreakpoint(PC)) {
		debug();
	} else if (isPCAtExitAddress()) {
		return true;
	} else {
		executeOneInstruction();
	}
	return false;
}

void CPU::execute() {
	while (!executeOne()) 
		;
}

