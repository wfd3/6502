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

#include <map>
#include <tuple>
#include <string>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

//#include <unistd.h>

#include "memory.h"

using Byte      = unsigned char;
using SByte     = signed char;
using Word      = unsigned int;
using Address_t = unsigned int;
using cMemory   = Memory<Address_t, Byte>;

class Cycles_t {
public:
	Cycles_t() {
		_c = 0;
		_emulateTimings = true;
	}

	unsigned long get() {
		return _c;
	}

	void operator=(const unsigned long c) {
		_c = c;
	}
	
	bool operator==(const Cycles_t &rhs) {
		return _c == rhs._c;
	}
	
	Cycles_t& operator++(int) {
		#if 0
		_c++;
		if (_emulateTimings) {
		 	auto millisecond = std::chrono::milliseconds(1);
			std::this_thread::sleep_for(millisecond);
		}
		#endif
		operator+=(1);
		return *this;
	}

	Cycles_t& operator+=(int i) {
		_c += i;
		if (_emulateTimings) {
			auto milliseconds = std::chrono::milliseconds(i);
			std::this_thread::sleep_for(milliseconds);
		}
		return *this;
	}

	Byte operator-(Cycles_t const& c) {
		if (c._c > _c)
			return 0;
		return _c - c._c;
	}

	Cycles_t operator+(Cycles_t const& c) {
		Cycles_t res;
		res = _c + c._c;
		return res;
	}

	bool operator>(const Cycles_t &c)  {
		return _c > c._c;
	}

	bool operator>(unsigned char c) const {
		return _c > c;
	}

	bool operator<(const Cycles_t &c) {
		return _c < c._c;
	}

	void enableTimingEmulation() {
		_emulateTimings = true;
	}
	
	void disableTimingEmulation() {
		_emulateTimings = false;
	}
	
private:
	unsigned long _c;
	bool _emulateTimings;
};


// https://archive.org/details/6500-50a_mcs6500pgmmanjan76/page/n1/mode/2up
// http://archive.6502.org/books/mcs6500_family_hardware_manual.pdf
	
class CPU {

public:

	constexpr static Word MAX_MEM          = 0xFFFF;
	constexpr static Byte INITIAL_SP       = 0xFF;
	constexpr static Word RESET_VECTOR     = 0xFFFC;
	constexpr static Word INTERRUPT_VECTOR = 0xFFFE;

	Word PC;		// Program counter
	Byte SP;		// Stack pointer
	Byte A, X, Y;		// Registers
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
		Byte PS;
		struct ProcessorStatusBits Flags;
	};

        CPU(cMemory *);
	void Reset(Word);
	void Reset();
	void exitReset();
	void setResetVector(Word);
	void setInterruptVector(Word);
	void execute();
	void debug();
	void setDebug(bool);
		
	std::tuple<Byte, Byte> executeOneInstruction();
	std::tuple<Byte, Byte> traceOneInstruction();

	bool isDebugEnabled() {
		return debugMode;
	}

	void setPendingReset() {
		pendingReset = true;
	}

	typedef void (*debugEntryExitFn_t)(void);
	void setDebugEntryExitFunc(debugEntryExitFn_t entryfn = NULL,
				   debugEntryExitFn_t exitfn = NULL) {

		debugEntryFunc = entryfn;
		debugExitFunc = exitfn;
	}

	void toggleLoopDetection() {
		debug_loopDetection = !debug_loopDetection;
	}

	void setExitAddress(Address_t _pc) {
		_exitAddress = _pc;
		_exitAddressSet = true;
	}

	void unsetExitAddress() {
		_exitAddressSet = false;
	}

private:
	cMemory *mem;
	bool pendingReset;
	bool overrideResetVector;
	Word pendingResetPC;

	// Instruction map
	typedef void (CPU::*opfn_t)(Byte, Byte &);
	struct instruction {
		const char *name;
	        Byte addrmode;
		Byte flags;
		Byte bytes;
		Byte cycles;
		opfn_t opfn;
	};
	std::map<Byte, instruction> instructions;

	CPU::instruction makeIns(const char *, Byte, Byte, Byte, Byte, opfn_t);
	void setupInstructionMap();

	// CPU functions
	void exception(const std::string &);
	void setFlagZ(Byte);
	void setFlagN(Byte);
	bool isNegative(Byte);
	void push(Byte);
	Byte pop();
	void pushWord(Word);
	Word popWord();
	Word getAddress(Byte, Byte &);
	Byte getData(Byte, Byte &);
	Byte readByteAtPC();
	Word readWordAtPC();
	void writeByte(Word, Byte);
	Byte readByte(Word);
	void writeWord(Word, Word);
	Word readWord(Word);
	void doBranch(bool, Word, Word, Byte &);
	void doADC(Byte);
	void bcdADC(Byte);
	void bcdSBC(Byte);
	void pushPS();
	void popPS();

	// Debugger
	bool debugMode;
	void toggleDebug();

	debugEntryExitFn_t debugEntryFunc, debugExitFunc;
	std::string debug_lastCmd;
	bool debug_alwaysShowPS;

	std::vector<Word> breakpoints;
	Address_t _exitAddress;
	bool _exitAddressSet;
	std::vector<std::string> backtrace;

	std::string decodeArgs(Byte opcode);
	Address_t disassemble(Address_t, unsigned long);
	Address_t disassembleAt(Address_t dPC, std::string &d);
	unsigned long debugPrompt();
	void dumpStack();
	void printCPUState();
	void parseMemCommand(std::string);
	void debuggerPrompt();

	bool debug_loopDetection;

	void listBreakpoints();
	bool isBreakpoint(Word);
	void deleteBreakpoint(Word);
	void addBreakpoint(Word);

	bool isPCAtExitAddress() {
		return _exitAddressSet && (PC == _exitAddress);
	}

	void showBacktrace();
	void addBacktrace(Word);
	void removeBacktrace();

public:
	// Opcodes definitions
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
	constexpr static Byte NONE             = 0;
	constexpr static Byte CYCLE_BRANCH     = 1;
	constexpr static Byte CYCLE_CROSS_PAGE = 2;

	// Bits for PS byte
	constexpr static Byte BreakBit    = 1 << 4;
	constexpr static Byte UnusedBit   = 1 << 5;
	constexpr static Byte NegativeBit = 1 << 7;

	// 6502 stack is one page at 01ff down to 0100.  This is the
	// stack frame for that page.
	constexpr static Word STACK_FRAME = 0x0100;

	// Instruction functions
	void ins_adc(Byte, Byte &);
	void ins_and(Byte, Byte &);
	void ins_asl(Byte, Byte &);
	void ins_bcc(Byte, Byte &);
	void ins_bcs(Byte, Byte &);
	void ins_beq(Byte, Byte &);
	void ins_bit(Byte, Byte &);
	void ins_bmi(Byte, Byte &);
	void ins_bne(Byte, Byte &);
	void ins_bpl(Byte, Byte &);
	void ins_brk(Byte, Byte &);
	void ins_bvc(Byte, Byte &);
	void ins_bvs(Byte, Byte &);
	void ins_clc(Byte, Byte &);
	void ins_cld(Byte, Byte &);
	void ins_cli(Byte, Byte &);
	void ins_clv(Byte, Byte &);
	void ins_cmp(Byte, Byte &);
	void ins_cpx(Byte, Byte &);
	void ins_cpy(Byte, Byte &);
	void ins_dec(Byte, Byte &);
	void ins_dex(Byte, Byte &);
	void ins_dey(Byte, Byte &);
	void ins_eor(Byte, Byte &);
	void ins_inc(Byte, Byte &);
	void ins_inx(Byte, Byte &);
	void ins_iny(Byte, Byte &);
	void ins_jmp(Byte, Byte &);
	void ins_jsr(Byte, Byte &);
	void ins_lda(Byte, Byte &);
	void ins_ldx(Byte, Byte &);
	void ins_ldy(Byte, Byte &);
	void ins_lsr(Byte, Byte &);
	void ins_nop(Byte, Byte &);
	void ins_ora(Byte, Byte &);
	void ins_pha(Byte, Byte &);
	void ins_pla(Byte, Byte &);
	void ins_php(Byte, Byte &);
	void ins_plp(Byte, Byte &);
	void ins_rol(Byte, Byte &);
	void ins_ror(Byte, Byte &);
	void ins_rti(Byte, Byte &);
	void ins_rts(Byte, Byte &);
	void ins_sbc(Byte, Byte &);
	void ins_sec(Byte, Byte &);
	void ins_sed(Byte, Byte &);
	void ins_sei(Byte, Byte &);
	void ins_sta(Byte, Byte &);
	void ins_stx(Byte, Byte &);
	void ins_sty(Byte, Byte &);
	void ins_tax(Byte, Byte &);
	void ins_tay(Byte, Byte &);
	void ins_tsx(Byte, Byte &);
	void ins_txa(Byte, Byte &);
	void ins_txs(Byte, Byte &);
	void ins_tya(Byte, Byte &);
};	
