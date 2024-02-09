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

#include <6502.h>

//////////
// CPU Setup and reset
MOS6502::MOS6502(Memory<Word, Byte>& m) : debugger(*this), _instructions(setupInstructionMap()), mem(m), _inReset(true) {}	

void MOS6502::setResetVector(const Word address) {
	writeWord(RESET_VECTOR, address);
}

void MOS6502::setInterruptVector(const Word address) {
	writeWord(INTERRUPT_VECTOR, address);
}

void MOS6502::setPendingReset() {
		if (!_debugMode)
			_pendingReset = true;
	}

bool MOS6502::inReset() { 
	return _inReset; 
}

void MOS6502::raiseIRQ() { 
	_pendingIRQ = true; 
}

void MOS6502::raiseNMI() { 
	_pendingNMI = true; 
}

bool MOS6502::pendingIRQ() { 
	return _pendingIRQ; 
}

bool MOS6502::pendingNMI() { 
	return _pendingNMI; 
}
	
void MOS6502::unsetHaltAddress() { 
	_haltAddressSet = false; 
}

void MOS6502::setHaltAddress(const Word _pc) {
	_haltAddress = _pc;
	_haltAddressSet = true;
}

bool MOS6502::isPCAtHaltAddress() {
	return _haltAddressSet && (PC == _haltAddress);
}

void MOS6502::enableLoopDetection(const bool l) {
	_infiniteLoopDetection = l;
}

bool MOS6502::isLoopDetectionEnabled() { 
	return _infiniteLoopDetection;
}

bool MOS6502::loopDetected() { 
	return _loopDetected;
}

bool MOS6502::isInDebugMode() { 
	return _debugMode; 
}

void MOS6502::setDebugMode(const bool m) { 
	_debugMode = m; 
}

bool MOS6502::hitException() { 
	return _hitException; 
}

Cycles_t MOS6502::expectedCycles() {
	return _expectedCyclesToUse; 
}

Cycles_t MOS6502::usedCycles() { 
	return _cycles; 
}

void MOS6502::exitReset() {
	PC = readWord(RESET_VECTOR);
	SP = INITIAL_SP;

#ifdef TEST_BUILD
	// If we're here via TestReset() clobber the PC and SP with test values
	if (_testReset) {
		SP = _testResetSP;
		PC = _testResetPC;
	}
	_testReset = false;
#endif

	_debugMode = false;
	debugger.setCPUStatusAtPrompt(false); // TODO: Need a debugger.Reset()? 

	_hitException = false;

	_inReset = false;
	_pendingReset = false;

	_cycles += 7;		
}	

// This is only intended for testing, not for emulation.  It allows tests to set specific starting
// Program Counter and Stack Pointer values, exits reset so that the next call to execute...() executes code.
#ifdef TEST_BUILD
void MOS6502::TestReset(const Word initialPC, const Byte initialSP)  {
	_inReset = true;
	_pendingReset = true;
	_testReset = true;
	_testResetPC = initialPC;
	_testResetSP = initialSP;
	Reset();
}
#endif

// 'Asserts' the Reset line if not asserted, de-asserts the Reset line if asserted. 
void MOS6502::Reset() {
	if (!_inReset) {		// Not in Reset, assert the Reset line
		_inReset = true;
	} else {				// In Reset, de-assert Reset
		_inReset = false;
		exitReset();
	}
}

//////////
// Interrupts

void MOS6502::interrupt() {

	pushWord(PC);
	pushPS();

	Flags.I = 1;
	PC = readWord(INTERRUPT_VECTOR);
	_cycles++;
}

bool MOS6502::NMI() {
	if (pendingNMI()) {
		debugger.addBacktraceInterrupt(PC);
		_IRQCount++;
		interrupt();
		_pendingNMI = false;
		return true;
	}

	return false;
}

bool MOS6502::IRQ() {
	if (pendingIRQ() && !IRQBlocked()) {
		debugger.addBacktraceInterrupt(PC);
		_NMICount++;
		interrupt();
		_pendingIRQ = false;
		return true;
	}

	return false;
}

//////////
// CPU Exception
void MOS6502::exception(const std::string& message) {
	std::string msg = "CPU Exception: " + message;
	_hitException = true;
	
	if (debugger.debugModeOnException() && !_debugMode) 
		_debugMode = true;

	throw std::runtime_error(msg);
}

//////////
// Flags
bool MOS6502::isNegative(const Byte val) {
	return (val & NegativeBit);
}

void MOS6502::setFlagNByValue(const Byte val) {
	Flags.N = isNegative(val);
}

void MOS6502::setFlagZByValue(const Byte val) {
	Flags.Z = (val == 0);
}

bool MOS6502::IRQBlocked() {
	return Flags.I == 1;
}

//////////
// Memory access
Byte MOS6502::readByte(const Word address) {
	Byte data = mem.Read(address);
	_cycles++;
	return data;
}

void MOS6502::writeByte(const Word address, const Byte value) {
	mem.Write(address, value);
	_cycles++;
}

Word MOS6502::readWord(const Word address) {
	Word w = readByte(address) | (readByte(address + 1) << 8);
	return w;
}

void MOS6502::writeWord(const Word address, const Word word) {
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
void MOS6502::push(const Byte value) {
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

void MOS6502::pushWord(const Word value) {
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
Word MOS6502::getAddress(const Byte opcode) {
	Word address;
	SByte rel;

	// Add a cycle if a page boundary is crossed
	auto updateCycles = [&](Byte reg) {
		if (( _instructions.at(opcode).flags & InstructionFlags::PageBoundary) &&
		    ((address + reg) >> 8) != (address >> 8)) {
			_expectedCyclesToUse++;
			_cycles++;
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
		_cycles++;
		break;

	// ZeroPage,Y (with zero page wrap around)
	case AddressingMode::ZeroPageY:
		address = static_cast<Byte>(readByteAtPC() + Y);
		_cycles++;
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
		_cycles++;
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

Byte MOS6502::getData(const Byte opcode) {
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
		address = getAddress(opcode);
		data = readByte(address);
		break;
	}

	return data;
}

//////////
// Instruction execution
void MOS6502::executeOneInstruction() {
	Byte opcode;
	Word startPC;
	struct instruction ins;

	if (hitException()) {
		fmt::print("CPU has hit an exception\n");
		return;
	}
	
 	if (_inReset)
		return;

	// Reset cycle count before checking if we need to exit reset.
	_cycles = 0; 

	if (_pendingReset) {
		exitReset();
	}

	if (isPCAtHaltAddress()) {
		fmt::print("At halt address {:04x}\n", PC);
		return;
	}

	// Saving the PC has to happen before readByteAtPC(), which consumes clock cycles and increments the PC
	startPC = PC;

	opcode = readByteAtPC();
	try {
		ins = _instructions.at(opcode);
	} 
	catch (...) {
		PC = startPC;
		auto s = fmt::format("Invalid opcode {:04x} at PC {:04x}", opcode, PC);
		exception(s);
		return;
	}

	_expectedCyclesToUse = ins.cycles;
	
	ins.opfn(opcode);

	if ( startPC == PC) {
		if (_loopDetected) 
			throw std::runtime_error("Recursive loop detected");
			
		if (_infiniteLoopDetection) 
			fmt::print("# Loop detected at {:04x}\n", PC);
		_loopDetected = true;
		return;
	}

	// Check for a pending Non-maskable interrupt.  If none then
	// check for a pending interrupt request.  
	if (!NMI())
		IRQ();
}

void MOS6502::execute() {

	if (debugger.isPCBreakpoint() && !_debugMode) {
		// Set debug mode and return so the caller can setup the terminal if needed.
		_debugMode = true;
		return;
	}

	if (_debugMode) {
		debugger.executeDebug();
		return;
	}

	executeOneInstruction();

	_debugMode |= loopDetected();
}

void MOS6502::printCPUState() {
	auto yesno = [](bool b) -> std::string {
		return b ? "Yes" : "No";
	};
	auto fl = [](char c, bool b) -> char {
		return b ? std::toupper(c) : std::tolower(c);
	};

	fmt::print("  | PC: {:04x} SP: {:02x}\n", PC, SP );
	// fmt::print() doesn't like to print out union/bit-field members?
	fmt::print("  | Flags: {}{}{}{}{}{}{} (PS: {:#x})\n",
		fl('C', Flags.C), fl('Z', Flags.Z), fl('I', Flags.I), fl('D', Flags.D),
		fl('B', Flags.B), fl('V', Flags.V), fl('N', Flags.N), PS);
	fmt::print("  | A: {:02x} X: {:02x} Y: {:02x}\n", A, X, Y );
	fmt::print("  | Pending: IRQ - {}, NMI - {}, Reset - {}; inReset? - {}\n",
		   yesno(pendingIRQ()), yesno(pendingNMI()), yesno(_pendingReset), yesno(_inReset));
	fmt::print("  | IRQs: {}, NMIs: {}, BRKs: {}\n",
		    _IRQCount, _NMICount, _BRKCount);
	fmt::print("\n");
}