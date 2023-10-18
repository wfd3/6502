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
#include <tuple>
#include <string>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <atomic>
#include <cstdint>

#include "memory.h"
#include "clock.h"

using Byte      = uint8_t;
using SByte     = int8_t;
using Word      = uint16_t;
using Address_t = uint16_t;
using cMemory   = Memory<Address_t, Byte>;

// Forward declaration with extern "C" linkage for later use as a
// friend in class CPU. This is for GNU Readline.
extern "C" char **readlineCompletionCallback(const char* text, int start,
					     int end);

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

class CPU {

////////////////////
// CPU
////////////////////	
public:
	Word PC = 0;		// Program counter
	Byte SP = 0;		// Stack pointer
	Byte A = 0;
	Byte X = 0;
	Byte Y = 0;		// Registers
	Cycles_t Cycles;	// Cycle counter
	struct ProcessorStatusBits {
		Byte C:1; 	// Carry (bit 0)
		Byte Z:1;	// Zero (bit 1)
		Byte I:1;	// Interrupt disable (bit 2)
		Byte D:1;	// Decimal mode (bit 3)
		Byte B:1;	// Break (bit 4)
		Byte _unused:1;	// Unused (bit 5)
		Byte V:1;	// Overflow (bit 6)
		Byte N:1;	// Negative (bit 7)
	};
	union {
		Byte PS = 0;
		struct ProcessorStatusBits Flags;
	};

	// CPU Setup & reset
	CPU(cMemory &);
	void Reset(Word initialPC, Byte initialSP = INITIAL_SP);
	void Reset();
	void setResetVector(Word);
	void setPendingReset() {
		if (!debugMode)
			_pendingReset = true;
	}
	bool inReset() { return _inReset; }

	void setInterruptVector(Word);
	void raiseIRQ() { _pendingIRQ = true; }
	void raiseNMI() { _pendingNMI = true; }
	bool pendingIRQ() { return _pendingIRQ; }
	bool pendingNMI() { return _pendingNMI; }

	void unsetExitAddress() { _exitAddressSet = false; }
	void setExitAddress(Address_t _pc) {
		_exitAddress = _pc;
		_exitAddressSet = true;
	}
	void toggleLoopDetection() {
		debug_loopDetection = !debug_loopDetection;
	}

	// Execution
	std::tuple<uint64_t, uint64_t> executeOneInstruction();
	bool executeOne();
	void execute();

private:
	// Setup & reset
	cMemory& mem;
	std::atomic_bool _inReset = false;      // CPU is held in reset
	std::atomic_bool _pendingReset = false;
	std::atomic_bool _pendingIRQ = false;
	std::atomic_bool _pendingNMI = false;
	bool _hitException = false;
	Word pendingResetPC = 0;
	Byte pendingResetSP = INITIAL_SP;

	Address_t _exitAddress = 0;
	bool _exitAddressSet = false;
	bool isPCAtExitAddress() {
		return _exitAddressSet && (PC == _exitAddress);
	}

	void exitReset();
	// Instruction map
	typedef void (CPU::*opfn_t)(Byte, uint64_t &);
	struct instruction {
		const char *name;
	    Byte addrmode;
		Byte bytes;
		Byte cycles;
		Byte flags;
		opfn_t opfn;
	};
	const std::map<Byte, instruction> _instructions;

	CPU::instruction makeIns(const char *, Byte, Byte, Byte, Byte, opfn_t);
	static std::map<Byte, instruction> setupInstructionMap();

	// CPU functions
	void exception(const std::string &);

	// Flags
	void setFlagZ(Byte);
	void setFlagN(Byte);
	bool isNegative(Byte);

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
	Word getAddress(Byte, uint64_t &);
	Byte getData(Byte, uint64_t &);

	// Interrupts
	void interrupt();
	bool NMI();
	bool IRQ();
	uint64_t _IRQCount = 0;
	uint64_t _NMICount = 0;
	uint64_t _BRKCount = 0;

	// Helper functions
	void doBranch(bool, Word, uint64_t &);
	void doADC(Byte);
	void bcdADC(Byte);
	void bcdSBC(Byte);

////////////////////
// CPU constants
////////////////////
public:
	// CPU addressable memory
	constexpr static Word MAX_MEM          = 0xFFFF;

	// CPU initial vectors
	// TODO: These could be private, will need
	// some way for the tests to access
	constexpr static Byte INITIAL_SP       = 0xFF; 
	constexpr static Word RESET_VECTOR     = 0xFFFC;
	constexpr static Word INTERRUPT_VECTOR = 0xFFFE;

	// 6502 Opcode definitions
	//  These are public because they're needed by the 
	//  tests. 
	constexpr static Byte INS_BRK_IMP = 0x00;
	constexpr static Byte INS_ORA_IDX = 0x01;
	constexpr static Byte INS_ASL_ACC = 0x0a;
	constexpr static Byte INS_ADC_ZP  = 0x65;
	constexpr static Byte INS_ROR_ZP  = 0x66;
	constexpr static Byte INS_ADC_IMM = 0x69;
	constexpr static Byte INS_ROR_ACC = 0x6a;
	constexpr static Byte INS_JMP_IND = 0x6c;
	constexpr static Byte INS_ADC_ABS = 0x6d;
	constexpr static Byte INS_ROR_ABS = 0x6e;
	constexpr static Byte INS_BVS_REL = 0x70;
	constexpr static Byte INS_ADC_IDY = 0x71;
	constexpr static Byte INS_ADC_ZPX = 0x75;
	constexpr static Byte INS_ROR_ZPX = 0x76;
	constexpr static Byte INS_SEI_IMP = 0x78;
	constexpr static Byte INS_ADC_ABY = 0x79;
	constexpr static Byte INS_ADC_ABX = 0x7d;
	constexpr static Byte INS_ROR_ABX = 0x7e;
	constexpr static Byte INS_STA_IDX = 0x81;
	constexpr static Byte INS_ORA_ABS = 0x0d;
	constexpr static Byte INS_STY_ZP  = 0x84;
	constexpr static Byte INS_STA_ZP  = 0x85;
	constexpr static Byte INS_STX_ZP  = 0x86;
	constexpr static Byte INS_DEY_IMP = 0x88;
	constexpr static Byte INS_TXA_IMP = 0x8a;
	constexpr static Byte INS_ASL_ABS = 0x0e;
	constexpr static Byte INS_STY_ABS = 0x8c;
	constexpr static Byte INS_STA_ABS = 0x8d;
	constexpr static Byte INS_STX_ABS = 0x8e;
	constexpr static Byte INS_BCC_REL = 0x90;
	constexpr static Byte INS_STA_IDY = 0x91;
	constexpr static Byte INS_STY_ZPX = 0x94;
	constexpr static Byte INS_STA_ZPX = 0x95;
	constexpr static Byte INS_STX_ZPY = 0x96;
	constexpr static Byte INS_TYA_IMP = 0x98;
	constexpr static Byte INS_STA_ABY = 0x99;
	constexpr static Byte INS_TXS_IMP = 0x9a;
	constexpr static Byte INS_STA_ABX = 0x9d;
	constexpr static Byte INS_BPL_REL = 0x10;
	constexpr static Byte INS_LDY_IMM = 0xa0;
	constexpr static Byte INS_LDA_IDX = 0xa1;
	constexpr static Byte INS_LDX_IMM = 0xa2;
	constexpr static Byte INS_LDY_ZP  = 0xa4;
	constexpr static Byte INS_LDA_ZP  = 0xa5;
	constexpr static Byte INS_LDX_ZP  = 0xa6;
	constexpr static Byte INS_TAY_IMP = 0xa8;
	constexpr static Byte INS_LDA_IMM = 0xa9;
	constexpr static Byte INS_ORA_IDY = 0x11;
	constexpr static Byte INS_TAX_IMP = 0xaa;
	constexpr static Byte INS_LDY_ABS = 0xac;
	constexpr static Byte INS_LDA_ABS = 0xad;
	constexpr static Byte INS_LDX_ABS = 0xae;
	constexpr static Byte INS_BCS_REL = 0xb0;
	constexpr static Byte INS_LDA_IDY = 0xb1;
	constexpr static Byte INS_LDY_ZPX = 0xb4;
	constexpr static Byte INS_LDA_ZPX = 0xb5;
	constexpr static Byte INS_LDX_ZPY = 0xb6;
	constexpr static Byte INS_CLV_IMP = 0xb8;
	constexpr static Byte INS_LDA_ABY = 0xb9;
	constexpr static Byte INS_TSX_IMP = 0xba;
	constexpr static Byte INS_LDY_ABX = 0xbc;
	constexpr static Byte INS_LDA_ABX = 0xbd;
	constexpr static Byte INS_LDX_ABY = 0xbe;
	constexpr static Byte INS_CPY_IMM = 0xc0;
	constexpr static Byte INS_CMP_IDX = 0xc1;
	constexpr static Byte INS_CPY_ZP  = 0xc4;
	constexpr static Byte INS_CMP_ZP  = 0xc5;
	constexpr static Byte INS_DEC_ZP  = 0xc6;
	constexpr static Byte INS_INY_IMP = 0xc8;
	constexpr static Byte INS_CMP_IMM = 0xc9;
	constexpr static Byte INS_DEX_IMP = 0xca;
	constexpr static Byte INS_CPY_ABS = 0xcc;
	constexpr static Byte INS_CMP_ABS = 0xcd;
	constexpr static Byte INS_DEC_ABS = 0xce;
	constexpr static Byte INS_BNE_REL = 0xd0;
	constexpr static Byte INS_CMP_IDY = 0xd1;
	constexpr static Byte INS_ORA_ZPX = 0x15;
	constexpr static Byte INS_CMP_ZPX = 0xd5;
	constexpr static Byte INS_DEC_ZPX = 0xd6;
	constexpr static Byte INS_CLD_IMP = 0xd8;
	constexpr static Byte INS_CMP_ABY = 0xd9;
	constexpr static Byte INS_ASL_ZPX = 0x16;
	constexpr static Byte INS_CMP_ABX = 0xdd;
	constexpr static Byte INS_DEC_ABX = 0xde;
	constexpr static Byte INS_CPX_IMM = 0xe0;
	constexpr static Byte INS_SBC_IDX = 0xe1;
	constexpr static Byte INS_CPX_ZP  = 0xe4;
	constexpr static Byte INS_SBC_ZP  = 0xe5;
	constexpr static Byte INS_INC_ZP  = 0xe6;
	constexpr static Byte INS_INX_IMP = 0xe8;
	constexpr static Byte INS_SBC_IMM = 0xe9;
	constexpr static Byte INS_NOP_IMP = 0xea;
	constexpr static Byte INS_CPX_ABS = 0xec;
	constexpr static Byte INS_SBC_ABS = 0xed;
	constexpr static Byte INS_INC_ABS = 0xee;
	constexpr static Byte INS_CLC_IMP = 0x18;
	constexpr static Byte INS_BEQ_REL = 0xf0;
	constexpr static Byte INS_SBC_IDY = 0xf1;
	constexpr static Byte INS_SBC_ZPX = 0xf5;
	constexpr static Byte INS_INC_ZPX = 0xf6;
	constexpr static Byte INS_SED_IMP = 0xf8;
	constexpr static Byte INS_SBC_ABY = 0xf9;
	constexpr static Byte INS_ORA_ABY = 0x19;
	constexpr static Byte INS_SBC_ABX = 0xfd;
	constexpr static Byte INS_INC_ABX = 0xfe;
	constexpr static Byte INS_ORA_ABX = 0x1d;
	constexpr static Byte INS_ASL_ABX = 0x1e;
	constexpr static Byte INS_JSR_ABS = 0x20;
	constexpr static Byte INS_AND_IDX = 0x21;
	constexpr static Byte INS_BIT_ZP  = 0x24;
	constexpr static Byte INS_AND_ZP  = 0x25;
	constexpr static Byte INS_ROL_ZP  = 0x26;
	constexpr static Byte INS_PLP_IMP = 0x28;
	constexpr static Byte INS_AND_IMM = 0x29;
	constexpr static Byte INS_ROL_ACC = 0x2a;
	constexpr static Byte INS_BIT_ABS = 0x2c;
	constexpr static Byte INS_AND_ABS = 0x2d;
	constexpr static Byte INS_ROL_ABS = 0x2e;
	constexpr static Byte INS_BMI_REL = 0x30;
	constexpr static Byte INS_AND_IDY = 0x31;
	constexpr static Byte INS_ORA_ZP  = 0x05;
	constexpr static Byte INS_AND_ZPX = 0x35;
	constexpr static Byte INS_ROL_ZPX = 0x36;
	constexpr static Byte INS_SEC_IMP = 0x38;
	constexpr static Byte INS_AND_ABY = 0x39;
	constexpr static Byte INS_ASL_ZP  = 0x06;
	constexpr static Byte INS_AND_ABX = 0x3d;
	constexpr static Byte INS_ROL_ABX = 0x3e;
	constexpr static Byte INS_RTI_IMP = 0x40;
	constexpr static Byte INS_EOR_IDX = 0x41;
	constexpr static Byte INS_EOR_ZP  = 0x45;
	constexpr static Byte INS_LSR_ZP  = 0x46;
	constexpr static Byte INS_PHA_IMP = 0x48;
	constexpr static Byte INS_EOR_IMM = 0x49;
	constexpr static Byte INS_LSR_ACC = 0x4a;
	constexpr static Byte INS_JMP_ABS = 0x4c;
	constexpr static Byte INS_EOR_ABS = 0x4d;
	constexpr static Byte INS_LSR_ABS = 0x4e;
	constexpr static Byte INS_PHP_IMP = 0x08;
	constexpr static Byte INS_BVC_REL = 0x50;
	constexpr static Byte INS_EOR_IDY = 0x51;
	constexpr static Byte INS_EOR_ZPX = 0x55;
	constexpr static Byte INS_LSR_ZPX = 0x56;
	constexpr static Byte INS_CLI_IMP = 0x58;
	constexpr static Byte INS_EOR_ABY = 0x59;
	constexpr static Byte INS_ORA_IMM = 0x09;
	constexpr static Byte INS_EOR_ABX = 0x5d;
	constexpr static Byte INS_LSR_ABX = 0x5e;
	constexpr static Byte INS_RTS_IMP = 0x60;
	constexpr static Byte INS_ADC_IDX = 0x61;
	constexpr static Byte INS_PLA_IMP = 0x68;

private:
	// Addressing modes
	constexpr static Byte ADDR_MODE_IMM = 0;  // Immediate
	constexpr static Byte ADDR_MODE_ZP  = 1;  // Zero Page
	constexpr static Byte ADDR_MODE_ZPX = 2;  // Zero Page,X
	constexpr static Byte ADDR_MODE_ZPY = 3;  // Zero Page,Y
	constexpr static Byte ADDR_MODE_REL = 4;  // Relative
	constexpr static Byte ADDR_MODE_ABS = 5;  // Absolute
	constexpr static Byte ADDR_MODE_ABX = 6;  // Absolute,X
	constexpr static Byte ADDR_MODE_ABY = 7;  // Absolute,y
	constexpr static Byte ADDR_MODE_IND = 8;  // Indirect
	constexpr static Byte ADDR_MODE_IDX = 9;  // Indexed Ind
	constexpr static Byte ADDR_MODE_IDY = 10; // Indirect Idx
	constexpr static Byte ADDR_MODE_IMP = 11; // Implied
	constexpr static Byte ADDR_MODE_ACC = 12; // Accumulator

        // How the CPU should add cycle counts on branches and when
        // instructions fetch data across page boundries.
	constexpr static Byte NONE         = 0;
	constexpr static Byte CYCLE_BRANCH = 1;
	constexpr static Byte CYCLE_PAGE   = 2;

	// Bits for PS byte
	constexpr static Byte BreakBit    = 1 << 4;
	constexpr static Byte UnusedBit   = 1 << 5;
	constexpr static Byte NegativeBit = 1 << 7;

	// 6502 stack is one page at 01ff down to 0100.  This is the
	// stack frame for that page.
	constexpr static Word STACK_FRAME = 0x0100;

	// Instruction implementations
	void ins_adc(Byte, uint64_t &);
	void ins_and(Byte, uint64_t &);
	void ins_asl(Byte, uint64_t &);
	void ins_bcc(Byte, uint64_t &);
	void ins_bcs(Byte, uint64_t &);
	void ins_beq(Byte, uint64_t &);
	void ins_bit(Byte, uint64_t &);
	void ins_bmi(Byte, uint64_t &);
	void ins_bne(Byte, uint64_t &);
	void ins_bpl(Byte, uint64_t &);
	void ins_brk(Byte, uint64_t &);
	void ins_bvc(Byte, uint64_t &);
	void ins_bvs(Byte, uint64_t &);
	void ins_clc(Byte, uint64_t &);
	void ins_cld(Byte, uint64_t &);
	void ins_cli(Byte, uint64_t &);
	void ins_clv(Byte, uint64_t &);
	void ins_cmp(Byte, uint64_t &);
	void ins_cpx(Byte, uint64_t &);
	void ins_cpy(Byte, uint64_t &);
	void ins_dec(Byte, uint64_t &);
	void ins_dex(Byte, uint64_t &);
	void ins_dey(Byte, uint64_t &);
	void ins_eor(Byte, uint64_t &);
	void ins_inc(Byte, uint64_t &);
	void ins_inx(Byte, uint64_t &);
	void ins_iny(Byte, uint64_t &);
	void ins_jmp(Byte, uint64_t &);
	void ins_jsr(Byte, uint64_t &);
	void ins_lda(Byte, uint64_t &);
	void ins_ldx(Byte, uint64_t &);
	void ins_ldy(Byte, uint64_t &);
	void ins_lsr(Byte, uint64_t &);
	void ins_nop(Byte, uint64_t &);
	void ins_ora(Byte, uint64_t &);
	void ins_pha(Byte, uint64_t &);
	void ins_pla(Byte, uint64_t &);
	void ins_php(Byte, uint64_t &);
	void ins_plp(Byte, uint64_t &);
	void ins_rol(Byte, uint64_t &);
	void ins_ror(Byte, uint64_t &);
	void ins_rti(Byte, uint64_t &);
	void ins_rts(Byte, uint64_t &);
	void ins_sbc(Byte, uint64_t &);
	void ins_sec(Byte, uint64_t &);
	void ins_sed(Byte, uint64_t &);
	void ins_sei(Byte, uint64_t &);
	void ins_sta(Byte, uint64_t &);
	void ins_stx(Byte, uint64_t &);
	void ins_sty(Byte, uint64_t &);
	void ins_tax(Byte, uint64_t &);
	void ins_tay(Byte, uint64_t &);
	void ins_tsx(Byte, uint64_t &);
	void ins_txa(Byte, uint64_t &);
	void ins_txs(Byte, uint64_t &);
	void ins_tya(Byte, uint64_t &);

///////////////////////////////
// Debugger
///////////////////////////////
public:
	void setDebug(bool);
	bool isDebugEnabled() { return debugMode; }
	void debug();
	bool debuggerOnException() { return debug_OnException; }
	void enterDebuggerOnException(bool t) { debug_OnException = t; }
	std::tuple<uint64_t, uint64_t> traceOneInstruction();

	// Callbacks to run before entering and exiting the debugger
	typedef void (*debugEntryExitFn_t)(void);
	void setDebugEntryExitFunc(debugEntryExitFn_t entryfn = nullptr,
				   debugEntryExitFn_t exitfn = nullptr) {

		debugEntryFunc = entryfn;
		debugExitFunc = exitfn;
	}

	// The GNU readline command completion needs access to the
	// _debugCommands vector.
	friend char *readlineCommandGenerator(const char*, int);
	friend char **readlineCompletionCallback(const char*, int, int);

private:
	// Disassembler
	void decodeArgs(bool, Byte, std::string &, std::string&, std::string&, std::string&);
	Address_t disassemble(Address_t, uint64_t);
	Address_t disassembleAt(Address_t dPC, std::string &);

	// Debugger
	bool debugMode = false;
	debugEntryExitFn_t debugEntryFunc = nullptr;
	debugEntryExitFn_t debugExitFunc = nullptr;
	std::string debug_lastCmd = "";
	bool debug_alwaysShowPS = false;
	bool debug_loopDetection = false;
	bool debug_OnException = false;

	void toggleDebug();
	uint64_t debugPrompt();
	void dumpStack();
	void printCPUState();
	void parseMemCommand(std::string);

	typedef int (CPU::*debugFn_t)(std::string &, uint64_t &);

	// Debugger commands
	struct debugCommand {
		const char *command;
		const char *shortcut;
		const CPU::debugFn_t func;
		const bool doFileCompletion;
		const std::string helpMsg;
	};
	const std::vector<debugCommand> _debugCommands;

	static std::vector<debugCommand> setupDebugCommands();
	bool matchCommand(const std::string &, debugFn_t &);
	int helpCmd(std::string &, uint64_t &);
	int listCmd(std::string &, uint64_t &);
	int loadCmd(std::string &, uint64_t &);
	int runCmd(std::string &, uint64_t &);
	int stackCmd(std::string &, uint64_t &);
	int breakpointCmd(std::string &, uint64_t &);
	int cpustateCmd(std::string &, uint64_t &);
	int autostateCmd(std::string &, uint64_t &);
	int resetListPCCmd(std::string &, uint64_t &);
	int memdumpCmd(std::string &, uint64_t &);
	int memmapCmd(std::string &, uint64_t &);
	int setCmd(std::string &, uint64_t &);
	int resetCmd(std::string &, uint64_t &);
	int continueCmd(std::string &, uint64_t &);
	int loopdetectCmd(std::string &, uint64_t &);
	int backtraceCmd(std::string &, uint64_t &);
	int labelCmd(std::string &, uint64_t &);
	int whereCmd(std::string &, uint64_t &);
	int watchCmd(std::string &, uint64_t &);
	int quitCmd(std::string &, uint64_t &);
	int findCmd(std::string &, uint64_t &);
	int clockCmd(std::string &, uint64_t &);
	int loadcmdCmd(std::string &, uint64_t &);

	// Breakpoints
	std::vector<bool> breakpoints;
	void listBreakpoints();
	bool isBreakpoint(Word);
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
};	
