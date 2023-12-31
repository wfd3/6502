// Class definition(s) for emulated 6502
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

#pragma once

#include <array>
#include <map>
#include <unordered_map>
#include <string>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <atomic>
#include <cstdint>

#include "memory.h"

// Types used by 6502
using Byte      = uint8_t;
using SByte     = int8_t;
using Word      = uint16_t;
using Address_t = uint16_t;
using Cycles_t  = uint8_t;
using cMemory   = Memory<Address_t, Byte>;

// Forward declaration with extern "C" linkage for later use as a
// friend in class CPU. This is for GNU Readline.
extern "C" char **readlineCompletionCallback(const char* text, int start, int end);

//
// The CPU class is broken up into three different sections:
// 1) Core CPU public and private API
// 2) CPU constants, public and private
// 3) Debugger public and private API
//
// Each section is separated by a comment block.
//
// Reference materials:
// https://archive.org/details/6500-50a_mcs6500pgmmanjan76/page/n1/mode/2up
// http://archive.6502.org/books/mcs6500_family_hardware_manual.pdf

class MOS6502 {

////////////////////
// CPU
////////////////////	
public:
	// Last addressable byte
	constexpr static Word MAX_MEM      = 0xFFFF;
	
	// CPU initial vectors
	constexpr static Byte INITIAL_SP   = 0xFF; 
	constexpr static Word RESET_VECTOR = 0xFFFC;

	// CPU Setup & reset
	MOS6502(cMemory &);
	void Reset();
	void setResetVector(Word);
	void setPendingReset() {
		if (!_debuggingEnabled)
			_pendingReset = true;
	}
	bool inReset() { return _inReset; }

	void setInterruptVector(Word);
	void raiseIRQ() { _pendingIRQ = true; }
	void raiseNMI() { _pendingNMI = true; }
	
	// Public only for tests
	bool pendingIRQ() { return _pendingIRQ; }
	bool pendingNMI() { return _pendingNMI; }

	void unsetHaltAddress() { _haltAddressSet = false; }
	void setHaltAddress(Address_t _pc) {
		_haltAddress = _pc;
		_haltAddressSet = true;
	}
	void loopDetection(bool l) {
		debug_loopDetection = l;
	}

	// Execution
	void execute(bool& stop, bool& startDebugOnNextInstruction, Cycles_t& cyclesUsed);
	void executeOneInstructionWithCycleCount(Cycles_t &, Cycles_t &);

	// Debugger
	bool executeDebug();


#ifdef TEST_BUILD
	void TestReset(Word initialPC = RESET_VECTOR, Byte initialSP = INITIAL_SP);
	void traceOneInstruction(Cycles_t &, Cycles_t &);

	Word getPC() { return PC; }
	Byte getSP() { return SP; } 
	Byte getA()  { return A;  }
	Byte getX()  { return X;  }
	Byte getY()  { return Y;  }
	Byte getPS() { return PS; }

	bool getFlagC() { return Flags.C; }
	bool getFlagZ() { return Flags.Z; }
	bool getFlagI() { return Flags.I; }
	bool getFlagD() { return Flags.D; }
	bool getFlagB() { return Flags.B; }
	bool getFlagV() { return Flags.V; }
	bool getFlagN() { return Flags.N; }

	void setPC(Word _PC) { PC = _PC; }
	void setSP(Byte _SP) { SP = _SP; }
	void setA(Byte _A)   { A = _A; }
	void setX(Byte _X)   { X = _X; }
	void setY(Byte _Y)   { Y = _Y; }
	void setPS(Byte _PS) { PS = _PS; }
	
	void setFlagC(bool _v) { Flags.C = _v ? 1 : 0; }
	void setFlagZ(bool _v) { Flags.Z = _v ? 1 : 0; }
	void setFlagI(bool _v) { Flags.I = _v ? 1 : 0; }
	void setFlagD(bool _v) { Flags.D = _v ? 1 : 0; }
	void setFlagB(bool _v) { Flags.B = _v ? 1 : 0; }
	void setFlagV(bool _v) { Flags.V = _v ? 1 : 0; }
	void setFlagN(bool _v) { Flags.N = _v ? 1 : 0; }
#endif

private:
	

	Word PC = 0;		 // Program counter
	Byte SP = 0;		 // Stack pointer
	Byte A  = 0;		 // Accumulator 
	Byte X  = 0;		 // X Register
	Byte Y  = 0;		 // Y Register
	
	struct ProcessorStatusBits {
		Byte C:1; 	 // Carry (bit 0)
		Byte Z:1;	 // Zero (bit 1)
		Byte I:1;	 // Interrupt disable (bit 2)
		Byte D:1;	 // Decimal mode (bit 3)
		Byte B:1;	 // Break (bit 4)
		Byte _unused:1;	 // Unused (bit 5)
		Byte V:1;	 // Overflow (bit 6)
		Byte N:1;	 // Negative (bit 7)
	};
	union {
		Byte PS = 0;
		struct ProcessorStatusBits Flags;
	};

	Cycles_t Cycles = 0;     // Cycle counter

	//////////
	// Special addresses/vectors
	
	// Interrupt/NMI vector
	constexpr static Word INTERRUPT_VECTOR = 0xFFFE;
	
	// 6502 stack is one page at 01ff, down to 0100.  This is the stack frame for that page.
	constexpr static Word STACK_FRAME = 0x0100;

	// Bits for PS byte
	constexpr static Byte BreakBit    = 1 << 4;
	constexpr static Byte UnusedBit   = 1 << 5;
	constexpr static Byte NegativeBit = 1 << 7;

	// Addressing modes
	enum class AddressingMode {
		Immediate,
		ZeroPage,
		ZeroPageX,
		ZeroPageY,
		Relative,
		Absolute,
		AbsoluteX,
		AbsoluteY,
		Indirect,
		IndirectX,
		IndirectY,
		Implied,
		Accumulator
	};
	
    	// How the CPU should add cycle counts on branches and when
    	// instructions fetch data across page boundaries.
	enum class InstructionFlags {
		None,
		Branch,
		PageBoundary
	};
	
	// Setup & reset
	cMemory& mem;
	std::atomic_bool _inReset = false;      // CPU is held in reset
	std::atomic_bool _pendingReset = false;
	std::atomic_bool _pendingIRQ = false;
	std::atomic_bool _pendingNMI = false;
	bool _hitException = false;
#ifdef TEST_BUILD	
	Word testResetPC = 0;
	Byte testResetSP = INITIAL_SP;
	bool _testReset = false;
#endif
	Address_t _haltAddress = 0;
	bool _haltAddressSet = false;

	void exitReset();
	bool isPCAtHaltAddress() {
		return _haltAddressSet && (PC == _haltAddress);
	}

	// Instruction map
	typedef void (MOS6502::*opfn_t)(Byte, Cycles_t &);
	struct instruction {
		const char *name;
	    AddressingMode addrmode;
		Byte bytes;
		Byte cycles;
		InstructionFlags flags;
		opfn_t opfn;
	};
	const std::map<Byte, instruction> _instructions;

	MOS6502::instruction makeIns(const char *, Byte, Byte, Byte, Byte, opfn_t);
	static std::map<Byte, instruction> setupInstructionMap();

	// CPU functions
	void exception(const std::string &);

	// Flags
	void setFlagZByValue(Byte);
	void setFlagNByValue(Byte);
	bool isNegative(Byte);
	bool IRQBlocked();

	// Stack operations
	void push(Byte);
	Byte pop();
	void pushWord(Word);
	Word popWord();
	void pushPS();
	void popPS();

	// Memory access	
	Byte readByteAtPC();
	Word readWordAtPC();
	void writeByte(Word, Byte);
	Byte readByte(Word);
	void writeWord(Word, Word);
	Word readWord(Word);

	// Address decoding
	Word getAddress(Byte, Cycles_t &);
	Byte getData(Byte, Cycles_t &);

	// Interrupts
	void interrupt();
	bool NMI();
	bool IRQ();
	uint64_t _IRQCount = 0;
	uint64_t _NMICount = 0;
	uint64_t _BRKCount = 0;

	// Instruction implementations
	void ins_adc(Byte, Cycles_t &);
	void ins_and(Byte, Cycles_t &);
	void ins_asl(Byte, Cycles_t &);
	void ins_bcc(Byte, Cycles_t &);
	void ins_bcs(Byte, Cycles_t &);
	void ins_beq(Byte, Cycles_t &);
	void ins_bit(Byte, Cycles_t &);
	void ins_bmi(Byte, Cycles_t &);
	void ins_bne(Byte, Cycles_t &);
	void ins_bpl(Byte, Cycles_t &);
	void ins_brk(Byte, Cycles_t &);
	void ins_bvc(Byte, Cycles_t &);
	void ins_bvs(Byte, Cycles_t &);
	void ins_clc(Byte, Cycles_t &);
	void ins_cld(Byte, Cycles_t &);
	void ins_cli(Byte, Cycles_t &);
	void ins_clv(Byte, Cycles_t &);
	void ins_cmp(Byte, Cycles_t &);
	void ins_cpx(Byte, Cycles_t &);
	void ins_cpy(Byte, Cycles_t &);
	void ins_dec(Byte, Cycles_t &);
	void ins_dex(Byte, Cycles_t &);
	void ins_dey(Byte, Cycles_t &);
	void ins_eor(Byte, Cycles_t &);
	void ins_inc(Byte, Cycles_t &);
	void ins_inx(Byte, Cycles_t &);
	void ins_iny(Byte, Cycles_t &);
	void ins_jmp(Byte, Cycles_t &);
	void ins_jsr(Byte, Cycles_t &);
	void ins_lda(Byte, Cycles_t &);
	void ins_ldx(Byte, Cycles_t &);
	void ins_ldy(Byte, Cycles_t &);
	void ins_lsr(Byte, Cycles_t &);
	void ins_nop(Byte, Cycles_t &);
	void ins_ora(Byte, Cycles_t &);
	void ins_pha(Byte, Cycles_t &);
	void ins_pla(Byte, Cycles_t &);
	void ins_php(Byte, Cycles_t &);
	void ins_plp(Byte, Cycles_t &);
	void ins_rol(Byte, Cycles_t &);
	void ins_ror(Byte, Cycles_t &);
	void ins_rti(Byte, Cycles_t &);
	void ins_rts(Byte, Cycles_t &);
	void ins_sbc(Byte, Cycles_t &);
	void ins_sec(Byte, Cycles_t &);
	void ins_sed(Byte, Cycles_t &);
	void ins_sei(Byte, Cycles_t &);
	void ins_sta(Byte, Cycles_t &);
	void ins_stx(Byte, Cycles_t &);
	void ins_sty(Byte, Cycles_t &);
	void ins_tax(Byte, Cycles_t &);
	void ins_tay(Byte, Cycles_t &);
	void ins_tsx(Byte, Cycles_t &);
	void ins_txa(Byte, Cycles_t &);
	void ins_txs(Byte, Cycles_t &);
	void ins_tya(Byte, Cycles_t &);

	// Helper functions for instruction implementations
	void doBranch(bool, Word, Cycles_t &);
	void doADC(Byte);
	void bcdADC(Byte);
	void bcdSBC(Byte);

	////
	// Built-in Debugger

	bool _debuggingEnabled = false;
	std::string debug_lastCmd = "";
	bool debug_alwaysShowPS = false;
	bool debug_loopDetection = false;
	bool debug_OnException = false;

	void initDebugger();
	bool executeDebuggerCmd(std::string);
	void dumpStack();
	void printCPUState();
	void parseMemCommand(std::string);
	
	// Disassembler
	void decodeArgs(bool, Byte, std::string &, std::string&, std::string&, std::string&);
	Address_t disassemble(Address_t, uint64_t);
	Address_t disassembleAt(Address_t dPC, std::string &);

	typedef bool (MOS6502::*debugFn_t)(std::string &);

	// Debugger commands
	struct debugCommand {
		const char *command;
		const char *shortcut;
		const MOS6502::debugFn_t func;
		const bool doFileCompletion;
		const std::string helpMsg;
	};
	const std::vector<debugCommand> _debugCommands;

	static std::vector<debugCommand> setupDebugCommands();
	bool matchCommand(const std::string &, debugFn_t &);
	bool helpCmd(std::string &);
	bool listCmd(std::string &);
	bool loadCmd(std::string &);
	bool stackCmd(std::string &);
	bool breakpointCmd(std::string &);
	bool cpustateCmd(std::string &);
	bool autostateCmd(std::string &);
	bool resetListPCCmd(std::string &);
	bool memdumpCmd(std::string &);
	bool memmapCmd(std::string &);
	bool setCmd(std::string &);
	bool resetCmd(std::string &);
	bool continueCmd(std::string &);
	bool loopdetectCmd(std::string &);
	bool backtraceCmd(std::string &);
	bool labelCmd(std::string &);
	bool whereCmd(std::string &);
	bool watchCmd(std::string &);
	bool quitCmd(std::string &);
	bool findCmd(std::string &);
	bool clockCmd(std::string &);
	bool loadScriptCmd(std::string &);
	bool savememCmd(std::string &);
	bool loadhexCmd(std::string &);

	// Hex file
	bool loadHexFile(const std::string&);
	bool saveToHexFile(const std::string&, const std::vector<std::pair<Word, Word>>&);
	bool saveToHexFile(const std::string&, Word startAddress, Word);

	// Breakpoints
	std::vector<bool> breakpoints;
	void listBreakpoints();
	bool isBreakpoint(Word);
	bool isPCBreakpoint() { return isBreakpoint(PC); }
	void deleteBreakpoint(Word);
	void addBreakpoint(Word);
	void deleteAllBreakpoints() { 
		breakpoints.assign(mem.size(), false);
		fmt::print("Deleted all breakpoints\n");
	}

	// Backtrace
	std::vector<std::string> backtrace;
	void showBacktrace();
	void addBacktrace(Word);
	void addBacktraceInterrupt(Word);
	void removeBacktrace();

	// Labels
	std::unordered_map<Word, std::string> addrToLabel;
	std::unordered_map<std::string, Word> labelToAddr;
	void showLabels();
	void addLabel(Word, const std::string);
	std::string addressLabel(const Word);
	std::string addressLabelSearch(const Word, const int8_t searchWidth = 3);
	bool labelAddress(const std::string&, Word&);
	void removeLabel(const Word);
	bool lookupAddress(const std::string&, Word&);
	bool parseCommandFile(const std::string&);
	std::string getLabelByte(const uint8_t);

	// GNU readline command completion, used by the debugger, needs access to the
	// _debugCommands vector.
	friend char *readlineCommandGenerator(const char*, int);
	friend char **readlineCompletionCallback(const char*, int, int);
};

// 6502 Opcode definitions
//   These are public because they're needed by the tests.
class Opcodes {
public:
	constexpr static Byte BRK_IMP = 0x00;
	constexpr static Byte ORA_IDX = 0x01;
	constexpr static Byte ASL_ACC = 0x0a;
	constexpr static Byte ADC_ZP  = 0x65;
	constexpr static Byte ROR_ZP  = 0x66;
	constexpr static Byte ADC_IMM = 0x69;
	constexpr static Byte ROR_ACC = 0x6a;
	constexpr static Byte JMP_IND = 0x6c;
	constexpr static Byte ADC_ABS = 0x6d;
	constexpr static Byte ROR_ABS = 0x6e;
	constexpr static Byte BVS_REL = 0x70;
	constexpr static Byte ADC_IDY = 0x71;
	constexpr static Byte ADC_ZPX = 0x75;
	constexpr static Byte ROR_ZPX = 0x76;
	constexpr static Byte SEI_IMP = 0x78;
	constexpr static Byte ADC_ABY = 0x79;
	constexpr static Byte ADC_ABX = 0x7d;
	constexpr static Byte ROR_ABX = 0x7e;
	constexpr static Byte STA_IDX = 0x81;
	constexpr static Byte ORA_ABS = 0x0d;
	constexpr static Byte STY_ZP  = 0x84;
	constexpr static Byte STA_ZP  = 0x85;
	constexpr static Byte STX_ZP  = 0x86;
	constexpr static Byte DEY_IMP = 0x88;
	constexpr static Byte TXA_IMP = 0x8a;
	constexpr static Byte ASL_ABS = 0x0e;
	constexpr static Byte STY_ABS = 0x8c;
	constexpr static Byte STA_ABS = 0x8d;
	constexpr static Byte STX_ABS = 0x8e;
	constexpr static Byte BCC_REL = 0x90;
	constexpr static Byte STA_IDY = 0x91;
	constexpr static Byte STY_ZPX = 0x94;
	constexpr static Byte STA_ZPX = 0x95;
	constexpr static Byte STX_ZPY = 0x96;
	constexpr static Byte TYA_IMP = 0x98;
	constexpr static Byte STA_ABY = 0x99;
	constexpr static Byte TXS_IMP = 0x9a;
	constexpr static Byte STA_ABX = 0x9d;
	constexpr static Byte BPL_REL = 0x10;
	constexpr static Byte LDY_IMM = 0xa0;
	constexpr static Byte LDA_IDX = 0xa1;
	constexpr static Byte LDX_IMM = 0xa2;
	constexpr static Byte LDY_ZP  = 0xa4;
	constexpr static Byte LDA_ZP  = 0xa5;
	constexpr static Byte LDX_ZP  = 0xa6;
	constexpr static Byte TAY_IMP = 0xa8;
	constexpr static Byte LDA_IMM = 0xa9;
	constexpr static Byte ORA_IDY = 0x11;
	constexpr static Byte TAX_IMP = 0xaa;
	constexpr static Byte LDY_ABS = 0xac;
	constexpr static Byte LDA_ABS = 0xad;
	constexpr static Byte LDX_ABS = 0xae;
	constexpr static Byte BCS_REL = 0xb0;
	constexpr static Byte LDA_IDY = 0xb1;
	constexpr static Byte LDY_ZPX = 0xb4;
	constexpr static Byte LDA_ZPX = 0xb5;
	constexpr static Byte LDX_ZPY = 0xb6;
	constexpr static Byte CLV_IMP = 0xb8;
	constexpr static Byte LDA_ABY = 0xb9;
	constexpr static Byte TSX_IMP = 0xba;
	constexpr static Byte LDY_ABX = 0xbc;
	constexpr static Byte LDA_ABX = 0xbd;
	constexpr static Byte LDX_ABY = 0xbe;
	constexpr static Byte CPY_IMM = 0xc0;
	constexpr static Byte CMP_IDX = 0xc1;
	constexpr static Byte CPY_ZP  = 0xc4;
	constexpr static Byte CMP_ZP  = 0xc5;
	constexpr static Byte DEC_ZP  = 0xc6;
	constexpr static Byte INY_IMP = 0xc8;
	constexpr static Byte CMP_IMM = 0xc9;
	constexpr static Byte DEX_IMP = 0xca;
	constexpr static Byte CPY_ABS = 0xcc;
	constexpr static Byte CMP_ABS = 0xcd;
	constexpr static Byte DEC_ABS = 0xce;
	constexpr static Byte BNE_REL = 0xd0;
	constexpr static Byte CMP_IDY = 0xd1;
	constexpr static Byte ORA_ZPX = 0x15;
	constexpr static Byte CMP_ZPX = 0xd5;
	constexpr static Byte DEC_ZPX = 0xd6;
	constexpr static Byte CLD_IMP = 0xd8;
	constexpr static Byte CMP_ABY = 0xd9;
	constexpr static Byte ASL_ZPX = 0x16;
	constexpr static Byte CMP_ABX = 0xdd;
	constexpr static Byte DEC_ABX = 0xde;
	constexpr static Byte CPX_IMM = 0xe0;
	constexpr static Byte SBC_IDX = 0xe1;
	constexpr static Byte CPX_ZP  = 0xe4;
	constexpr static Byte SBC_ZP  = 0xe5;
	constexpr static Byte INC_ZP  = 0xe6;
	constexpr static Byte INX_IMP = 0xe8;
	constexpr static Byte SBC_IMM = 0xe9;
	constexpr static Byte NOP_IMP = 0xea;
	constexpr static Byte CPX_ABS = 0xec;
	constexpr static Byte SBC_ABS = 0xed;
	constexpr static Byte INC_ABS = 0xee;
	constexpr static Byte CLC_IMP = 0x18;
	constexpr static Byte BEQ_REL = 0xf0;
	constexpr static Byte SBC_IDY = 0xf1;
	constexpr static Byte SBC_ZPX = 0xf5;
	constexpr static Byte INC_ZPX = 0xf6;
	constexpr static Byte SED_IMP = 0xf8;
	constexpr static Byte SBC_ABY = 0xf9;
	constexpr static Byte ORA_ABY = 0x19;
	constexpr static Byte SBC_ABX = 0xfd;
	constexpr static Byte INC_ABX = 0xfe;
	constexpr static Byte ORA_ABX = 0x1d;
	constexpr static Byte ASL_ABX = 0x1e;
	constexpr static Byte JSR_ABS = 0x20;
	constexpr static Byte AND_IDX = 0x21;
	constexpr static Byte BIT_ZP  = 0x24;
	constexpr static Byte AND_ZP  = 0x25;
	constexpr static Byte ROL_ZP  = 0x26;
	constexpr static Byte PLP_IMP = 0x28;
	constexpr static Byte AND_IMM = 0x29;
	constexpr static Byte ROL_ACC = 0x2a;
	constexpr static Byte BIT_ABS = 0x2c;
	constexpr static Byte AND_ABS = 0x2d;
	constexpr static Byte ROL_ABS = 0x2e;
	constexpr static Byte BMI_REL = 0x30;
	constexpr static Byte AND_IDY = 0x31;
	constexpr static Byte ORA_ZP  = 0x05;
	constexpr static Byte AND_ZPX = 0x35;
	constexpr static Byte ROL_ZPX = 0x36;
	constexpr static Byte SEC_IMP = 0x38;
	constexpr static Byte AND_ABY = 0x39;
	constexpr static Byte ASL_ZP  = 0x06;
	constexpr static Byte AND_ABX = 0x3d;
	constexpr static Byte ROL_ABX = 0x3e;
	constexpr static Byte RTI_IMP = 0x40;
	constexpr static Byte EOR_IDX = 0x41;
	constexpr static Byte EOR_ZP  = 0x45;
	constexpr static Byte LSR_ZP  = 0x46;
	constexpr static Byte PHA_IMP = 0x48;
	constexpr static Byte EOR_IMM = 0x49;
	constexpr static Byte LSR_ACC = 0x4a;
	constexpr static Byte JMP_ABS = 0x4c;
	constexpr static Byte EOR_ABS = 0x4d;
	constexpr static Byte LSR_ABS = 0x4e;
	constexpr static Byte PHP_IMP = 0x08;
	constexpr static Byte BVC_REL = 0x50;
	constexpr static Byte EOR_IDY = 0x51;
	constexpr static Byte EOR_ZPX = 0x55;
	constexpr static Byte LSR_ZPX = 0x56;
	constexpr static Byte CLI_IMP = 0x58;
	constexpr static Byte EOR_ABY = 0x59;
	constexpr static Byte ORA_IMM = 0x09;
	constexpr static Byte EOR_ABX = 0x5d;
	constexpr static Byte LSR_ABX = 0x5e;
	constexpr static Byte RTS_IMP = 0x60;
	constexpr static Byte ADC_IDX = 0x61;
	constexpr static Byte PLA_IMP = 0x68;
};
