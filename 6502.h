#ifndef __6502_h__
#define __6502_h__

#include <stdio.h>
#include <map>
#include <bitset>
#include <tuple>

namespace mos6502 {
	using Byte = unsigned char;
	using SByte = signed char;
	using Word = unsigned short;
	using Cycles_t = unsigned long;

	struct MEMORY;
	struct CPU;
}

const unsigned int MAX_MEM = 64 * 1024;

struct mos6502::MEMORY {
	mos6502::Byte m[MAX_MEM];

	void exception() {
		printf("Memory exception!\n");
		throw -1;
	}

	mos6502::Byte& operator[](mos6502::Word address) {
		if (address > MAX_MEM) {
			printf("Memory out of range (indexed 0x%x\n", address);
			exception();
		}
		return m[address];
	}

	void Init() {
		unsigned long i;
		for (i = 0; i < MAX_MEM; i++)
			m[i] = 0;
	}

	mos6502::Byte ReadByte(mos6502::Word address) {
		if (address > MAX_MEM)
			exception();
		return m[address];
	}

	void WriteByte(mos6502::Word address, mos6502::Byte byte) {
		if (address > MAX_MEM)
			exception();
		m[address] = byte;
	}

	mos6502::Word ReadWord(mos6502::Word address) {
		return ReadByte(address) | (ReadByte(address + 1) << 8);
	}

	void WriteWord(mos6502::Word address, mos6502::Word word) {
		WriteByte(address, word & 0xff);
		WriteByte(address + 1, word >> 8);
	}
};

struct mos6502::CPU {
	mos6502::Word PC;
	mos6502::Byte SP;
	mos6502::Byte A, X, Y;

	struct ProcessorStatusBits {
		mos6502::Byte C:1;
		mos6502::Byte Z:1;
		mos6502::Byte I:1;
		mos6502::Byte D:1;
		mos6502::Byte B:1;
		mos6502::Byte UNUSED:1;
		mos6502::Byte V:1;
		mos6502::Byte N:1;
	};
	union {
		mos6502::Byte PS;
		struct ProcessorStatusBits Flags;
	};

	mos6502::Cycles_t Cycles;

	constexpr static mos6502::Byte INITIAL_SP = 0xFF;
	constexpr static mos6502::Word STACK_FRAME = 0x0100;
	constexpr static mos6502::Word INITIAL_PC = 0xFFFC;

	void Reset(mos6502::Word);
	void Exception();
	void SetFlagsForRegister(mos6502::Byte b);
	void SetFlagsForCompare(mos6502::Byte b, mos6502::Byte v);
	mos6502::Byte FetchIns();
	void Push(mos6502::Byte);
	mos6502::Byte Pop();
	void PushWord(mos6502::Word);
	mos6502::Word PopWord();
	std::tuple<mos6502::Cycles_t, mos6502::Cycles_t> ExecuteOneInstruction();
	mos6502::Word getAddress(unsigned long, mos6502::Byte &);
	mos6502::Byte getData(unsigned long, mos6502::Byte &);

	// Opcodes
	constexpr static mos6502::Byte INS_BRK_IMP = 0x00;
	constexpr static mos6502::Byte INS_ORA_IDX = 0x01;
	constexpr static mos6502::Byte INS_ASL_ACC = 0x0a;
	constexpr static mos6502::Byte INS_ADC_ZP  = 0x65;
	constexpr static mos6502::Byte INS_ROR_ZP  = 0x66;
	constexpr static mos6502::Byte INS_ADC_IMM = 0x69;
	constexpr static mos6502::Byte INS_ROR_ACC = 0x6a;
	constexpr static mos6502::Byte INS_JMP_IND = 0x6c;
	constexpr static mos6502::Byte INS_ADC_ABS = 0x6d;
	constexpr static mos6502::Byte INS_ROR_ABS = 0x6e;
	constexpr static mos6502::Byte INS_BVS_REL = 0x70;
	constexpr static mos6502::Byte INS_ADC_IDY = 0x71;
	constexpr static mos6502::Byte INS_ADC_ZPX = 0x75;
	constexpr static mos6502::Byte INS_ROR_ZPX = 0x76;
	constexpr static mos6502::Byte INS_SEI_IMP = 0x78;
	constexpr static mos6502::Byte INS_ADC_ABY = 0x79;
	constexpr static mos6502::Byte INS_ADC_ABX = 0x7d;
	constexpr static mos6502::Byte INS_ROR_ABX = 0x7e;
	constexpr static mos6502::Byte INS_STA_IDX = 0x81;
	constexpr static mos6502::Byte INS_ORA_ABS = 0x0d;
	constexpr static mos6502::Byte INS_STY_ZP  = 0x84;
	constexpr static mos6502::Byte INS_STA_ZP  = 0x85;
	constexpr static mos6502::Byte INS_STX_ZP  = 0x86;
	constexpr static mos6502::Byte INS_DEY_IMP = 0x88;
	constexpr static mos6502::Byte INS_TXA_IMP = 0x8a;
	constexpr static mos6502::Byte INS_ASL_ABS = 0x0e;
	constexpr static mos6502::Byte INS_STY_ABS = 0x8c;
	constexpr static mos6502::Byte INS_STA_ABS = 0x8d;
	constexpr static mos6502::Byte INS_STX_ABS = 0x8e;
	constexpr static mos6502::Byte INS_BCC_REL = 0x90;
	constexpr static mos6502::Byte INS_STA_IDY = 0x91;
	constexpr static mos6502::Byte INS_STY_ZPX = 0x94;
	constexpr static mos6502::Byte INS_STA_ZPX = 0x95;
	constexpr static mos6502::Byte INS_STX_ZPY = 0x96;
	constexpr static mos6502::Byte INS_TYA_IMP = 0x98;
	constexpr static mos6502::Byte INS_STA_ABY = 0x99;
	constexpr static mos6502::Byte INS_TXS_IMP = 0x9a;
	constexpr static mos6502::Byte INS_STA_ABX = 0x9d;
	constexpr static mos6502::Byte INS_BPL_REL = 0x10;
	constexpr static mos6502::Byte INS_LDY_IMM = 0xa0;
	constexpr static mos6502::Byte INS_LDA_IDX = 0xa1;
	constexpr static mos6502::Byte INS_LDX_IMM = 0xa2;
	constexpr static mos6502::Byte INS_LDY_ZP  = 0xa4;
	constexpr static mos6502::Byte INS_LDA_ZP  = 0xa5;
	constexpr static mos6502::Byte INS_LDX_ZP  = 0xa6;
	constexpr static mos6502::Byte INS_TAY_IMP = 0xa8;
	constexpr static mos6502::Byte INS_LDA_IMM = 0xa9;
	constexpr static mos6502::Byte INS_ORA_IDY = 0x11;
	constexpr static mos6502::Byte INS_TAX_IMP = 0xaa;
	constexpr static mos6502::Byte INS_LDY_ABS = 0xac;
	constexpr static mos6502::Byte INS_LDA_ABS = 0xad;
	constexpr static mos6502::Byte INS_LDX_ABS = 0xae;
	constexpr static mos6502::Byte INS_BCS_REL = 0xb0;
	constexpr static mos6502::Byte INS_LDA_IDY = 0xb1;
	constexpr static mos6502::Byte INS_LDY_ZPX = 0xb4;
	constexpr static mos6502::Byte INS_LDA_ZPX = 0xb5;
	constexpr static mos6502::Byte INS_LDX_ZPY = 0xb6;
	constexpr static mos6502::Byte INS_CLV_IMP = 0xb8;
	constexpr static mos6502::Byte INS_LDA_ABY = 0xb9;
	constexpr static mos6502::Byte INS_TSX_IMP = 0xba;
	constexpr static mos6502::Byte INS_LDY_ABX = 0xbc;
	constexpr static mos6502::Byte INS_LDA_ABX = 0xbd;
	constexpr static mos6502::Byte INS_LDX_ABY = 0xbe;
	constexpr static mos6502::Byte INS_CPY_IMM = 0xc0;
	constexpr static mos6502::Byte INS_CMP_IDX = 0xc1;
	constexpr static mos6502::Byte INS_CPY_ZP  = 0xc4;
	constexpr static mos6502::Byte INS_CMP_ZP  = 0xc5;
	constexpr static mos6502::Byte INS_DEC_ZP  = 0xc6;
	constexpr static mos6502::Byte INS_INY_IMP = 0xc8;
	constexpr static mos6502::Byte INS_CMP_IMM = 0xc9;
	constexpr static mos6502::Byte INS_DEX_IMP = 0xca;
	constexpr static mos6502::Byte INS_CPY_ABS = 0xcc;
	constexpr static mos6502::Byte INS_CMP_ABS = 0xcd;
	constexpr static mos6502::Byte INS_DEC_ABS = 0xce;
	constexpr static mos6502::Byte INS_BNE_REL = 0xd0;
	constexpr static mos6502::Byte INS_CMP_IDY = 0xd1;
	constexpr static mos6502::Byte INS_ORA_ZPX = 0x15;
	constexpr static mos6502::Byte INS_CMP_ZPX = 0xd5;
	constexpr static mos6502::Byte INS_DEC_ZPX = 0xd6;
	constexpr static mos6502::Byte INS_CLD_IMP = 0xd8;
	constexpr static mos6502::Byte INS_CMP_ABY = 0xd9;
	constexpr static mos6502::Byte INS_ASL_ZPX = 0x16;
	constexpr static mos6502::Byte INS_CMP_ABX = 0xdd;
	constexpr static mos6502::Byte INS_DEC_ABX = 0xde;
	constexpr static mos6502::Byte INS_CPX_IMM = 0xe0;
	constexpr static mos6502::Byte INS_SBC_IDX = 0xe1;
	constexpr static mos6502::Byte INS_CPX_ZP  = 0xe4;
	constexpr static mos6502::Byte INS_SBC_ZP  = 0xe5;
	constexpr static mos6502::Byte INS_INC_ZP  = 0xe6;
	constexpr static mos6502::Byte INS_INX_IMP = 0xe8;
	constexpr static mos6502::Byte INS_SBC_IMM = 0xe9;
	constexpr static mos6502::Byte INS_NOP_IMP = 0xea;
	constexpr static mos6502::Byte INS_CPX_ABS = 0xec;
	constexpr static mos6502::Byte INS_SBC_ABS = 0xed;
	constexpr static mos6502::Byte INS_INC_ABS = 0xee;
	constexpr static mos6502::Byte INS_CLC_IMP = 0x18;
	constexpr static mos6502::Byte INS_BEQ_REL = 0xf0;
	constexpr static mos6502::Byte INS_SBC_IDY = 0xf1;
	constexpr static mos6502::Byte INS_SBC_ZPX = 0xf5;
	constexpr static mos6502::Byte INS_INC_ZPX = 0xf6;
	constexpr static mos6502::Byte INS_SED_IMP = 0xf8;
	constexpr static mos6502::Byte INS_SBC_ABY = 0xf9;
	constexpr static mos6502::Byte INS_ORA_ABY = 0x19;
	constexpr static mos6502::Byte INS_SBC_ABX = 0xfd;
	constexpr static mos6502::Byte INS_INC_ABX = 0xfe;
	constexpr static mos6502::Byte INS_ORA_ABX = 0x1d;
	constexpr static mos6502::Byte INS_ASL_ABX = 0x1e;
	constexpr static mos6502::Byte INS_JSR_ABS = 0x20;
	constexpr static mos6502::Byte INS_AND_IDX = 0x21;
	constexpr static mos6502::Byte INS_BIT_ZP  = 0x24;
	constexpr static mos6502::Byte INS_AND_ZP  = 0x25;
	constexpr static mos6502::Byte INS_ROL_ZP  = 0x26;
	constexpr static mos6502::Byte INS_PLP_IMP = 0x28;
	constexpr static mos6502::Byte INS_AND_IMM = 0x29;
	constexpr static mos6502::Byte INS_ROL_ACC = 0x2a;
	constexpr static mos6502::Byte INS_BIT_ABS = 0x2c;
	constexpr static mos6502::Byte INS_AND_ABS = 0x2d;
	constexpr static mos6502::Byte INS_ROL_ABS = 0x2e;
	constexpr static mos6502::Byte INS_BMI_REL = 0x30;
	constexpr static mos6502::Byte INS_AND_IDY = 0x31;
	constexpr static mos6502::Byte INS_ORA_ZP  = 0x05;
	constexpr static mos6502::Byte INS_AND_ZPX = 0x35;
	constexpr static mos6502::Byte INS_ROL_ZPX = 0x36;
	constexpr static mos6502::Byte INS_SEC_IMP = 0x38;
	constexpr static mos6502::Byte INS_AND_ABY = 0x39;
	constexpr static mos6502::Byte INS_ASL_ZP  = 0x06;
	constexpr static mos6502::Byte INS_AND_ABX = 0x3d;
	constexpr static mos6502::Byte INS_ROL_ABX = 0x3e;
	constexpr static mos6502::Byte INS_RTI_IMP = 0x40;
	constexpr static mos6502::Byte INS_EOR_IDX = 0x41;
	constexpr static mos6502::Byte INS_EOR_ZP  = 0x45;
	constexpr static mos6502::Byte INS_LSR_ZP  = 0x46;
	constexpr static mos6502::Byte INS_PHA_IMP = 0x48;
	constexpr static mos6502::Byte INS_EOR_IMM = 0x49;
	constexpr static mos6502::Byte INS_LSR_ACC = 0x4a;
	constexpr static mos6502::Byte INS_JMP_ABS = 0x4c;
	constexpr static mos6502::Byte INS_EOR_ABS = 0x4d;
	constexpr static mos6502::Byte INS_LSR_ABS = 0x4e;
	constexpr static mos6502::Byte INS_PHP_IMP = 0x08;
	constexpr static mos6502::Byte INS_BVC_REL = 0x50;
	constexpr static mos6502::Byte INS_EOR_IDY = 0x51;
	constexpr static mos6502::Byte INS_EOR_ZPX = 0x55;
	constexpr static mos6502::Byte INS_LSR_ZPX = 0x56;
	constexpr static mos6502::Byte INS_CLI_IMP = 0x58;
	constexpr static mos6502::Byte INS_EOR_ABY = 0x59;
	constexpr static mos6502::Byte INS_ORA_IMM = 0x09;
	constexpr static mos6502::Byte INS_EOR_ABX = 0x5d;
	constexpr static mos6502::Byte INS_LSR_ABX = 0x5e;
	constexpr static mos6502::Byte INS_RTS_IMP = 0x60;
	constexpr static mos6502::Byte INS_ADC_IDX = 0x61;
	constexpr static mos6502::Byte INS_PLA_IMP = 0x68;


	// Todo -- move to private section?
	typedef void (*opfn_t)(CPU *, unsigned long, mos6502::Byte &);
	struct instruction {
		const char *name;
		unsigned long addrmode;
		mos6502::Byte cycles;
		opfn_t opfn;
	};
	std::map<mos6502::Byte, instruction> instructions;
	void WriteByte(mos6502::Word, mos6502::Byte);

private:
	mos6502::Byte ReadByte(mos6502::Word);
	mos6502::Word ReadWord(mos6502::Word);
};

#endif
	
