// Class definition(s) for emulated 65C02 and Rockwell 65C02
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

class MOS65C02 : public MOS6502 {
public:
    MOS65C02(Memory<Word, Byte>& m) : MOS6502(m)  {
		_instructions = setup65C02Instructions();
	}

	// Must be public so the tests can access
    class OpcodeConstants : public MOS6502::OpcodeConstants {
    public:
		static constexpr Byte BRK_IMM = 0x00;

        static constexpr Byte ADC_ABS = 0x6d;
        static constexpr Byte ADC_ABX = 0x7d;
        static constexpr Byte ADC_ABY = 0x79;
        static constexpr Byte ADC_IDX = 0x61;
        static constexpr Byte ADC_IDY = 0x71;
        static constexpr Byte ADC_IMM = 0x69;
        static constexpr Byte ADC_ZP  = 0x65;
        static constexpr Byte ADC_ZPI = 0x72;
        static constexpr Byte ADC_ZPX = 0x75;
        
		static constexpr Byte AND_ZPI = 0x32;
        
		static constexpr Byte ASL_ABS = 0x0e;
        static constexpr Byte ASL_ABX = 0x1e;
        static constexpr Byte ASL_ZP  = 0x06;
        static constexpr Byte ASL_ZPX = 0x16;
        
		static constexpr Byte BIT_ABX = 0x3c;
        static constexpr Byte BIT_IMM = 0x89;
        static constexpr Byte BIT_ZPX = 0x34;
        
		static constexpr Byte BRA_REL = 0x80;
        
		static constexpr Byte CMP_ZPI = 0xd2;
        
		static constexpr Byte DEC_ACC = 0x3a;
        
		static constexpr Byte EOR_ZPI = 0x52;
        
		static constexpr Byte INC_ABX = 0xfe;
        static constexpr Byte INC_ACC = 0x1a;
        
		static constexpr Byte JMP_ABS = 0x4c;
        static constexpr Byte JMP_AII = 0x7c;
        static constexpr Byte JMP_IND = 0x6c;
        
		static constexpr Byte LDA_ZPI = 0xb2;
        
		static constexpr Byte LSR_ABX = 0x5e;
        
		static constexpr Byte ORA_ZPI = 0x12;
        
		static constexpr Byte PHX_IMP = 0xda;
        static constexpr Byte PHY_IMP = 0x5a;
        
		static constexpr Byte PLX_IMP = 0xfa;
        static constexpr Byte PLY_IMP = 0x7a;
        
		static constexpr Byte ROL_ABX = 0x3e;
        static constexpr Byte ROR_ABX = 0x7e;
        
		static constexpr Byte SBC_ABS = 0xed;
        static constexpr Byte SBC_ABX = 0xfd;
        static constexpr Byte SBC_ABY = 0xf9;
        static constexpr Byte SBC_IDX = 0xe1;
        static constexpr Byte SBC_IDY = 0xf1;
        static constexpr Byte SBC_IMM = 0xe9;
        static constexpr Byte SBC_ZP  = 0xe5;
        static constexpr Byte SBC_ZPI = 0xf2;
        static constexpr Byte SBC_ZPX = 0xf5;
        
		static constexpr Byte STA_ZPI = 0x92;

        static constexpr Byte STZ_ABS = 0x9c;
        static constexpr Byte STZ_ABX = 0x9e;
        static constexpr Byte STZ_ZP  = 0x64;
        static constexpr Byte STZ_ZPX = 0x74;

        static constexpr Byte TRB_ABS = 0x1c;
        static constexpr Byte TRB_ZP  = 0x14;

        static constexpr Byte TSB_ABS = 0x0c;
       	static constexpr Byte TSB_ZP  = 0x04;

		// R65C02 opcodes
		static constexpr Byte BBR0    = 0x0f;
		static constexpr Byte BBR1    = 0x1f;
		static constexpr Byte BBR2    = 0x2f;
		static constexpr Byte BBR3    = 0x3f;
		static constexpr Byte BBR4    = 0x4f;
		static constexpr Byte BBR5    = 0x5f;
		static constexpr Byte BBR6    = 0x6f;
		static constexpr Byte BBR7    = 0x7f;

		static constexpr Byte BBS0    = 0x8f;
		static constexpr Byte BBS1    = 0x9f;
		static constexpr Byte BBS2    = 0xaf;
		static constexpr Byte BBS3    = 0xbf;
		static constexpr Byte BBS4    = 0xcf;
		static constexpr Byte BBS5    = 0xdf;
		static constexpr Byte BBS6    = 0xef;
		static constexpr Byte BBS7    = 0xff;

		static constexpr Byte RMB0    = 0x07;
		static constexpr Byte RMB1    = 0x17;
		static constexpr Byte RMB2    = 0x27;
		static constexpr Byte RMB3    = 0x37;
		static constexpr Byte RMB4    = 0x47;
		static constexpr Byte RMB5    = 0x57;
		static constexpr Byte RMB6    = 0x67;
		static constexpr Byte RMB7    = 0x77;

		static constexpr Byte SMB0    = 0x87;
		static constexpr Byte SMB1    = 0x97;
		static constexpr Byte SMB2    = 0xa7;
		static constexpr Byte SMB3    = 0xb7;
		static constexpr Byte SMB4    = 0xc7;
		static constexpr Byte SMB5    = 0xd7;
		static constexpr Byte SMB6    = 0xe7;
		static constexpr Byte SMB7    = 0xf7;
    };
	OpcodeConstants Opcodes;

private:

    class InstructionFlags : public MOS6502::InstructionFlags {
	public:
        static constexpr uint8_t NoBoundaryCrossed = 4;
	};

	enum AddressingMode {
		// 6502 modes
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
		Accumulator,
		// 65C02 modes
		ZeroPageIndirect,
		AbsoluteIndexedIndirect
	};
	
	MOS6502::AddressingMode convertAddressingMode(AddressingMode);
	bool instructionIsAddressingMode(Byte, AddressingMode);
	Word getAddress(Byte);
	void decodeArgs(Word&, const bool, const Byte, std::string&, std::string&, std::string&, std::string&);
	void decodeRockwellArgs(Word&, std::string&, std::string&, std::string&);


	// 65C02 specific instructions
	void ins_bra(Byte);
	void ins_stz(Byte);
	void ins_trb(Byte);
	void ins_tsb(Byte);
	void ins_phx(Byte);
	void ins_phy(Byte);
	void ins_plx(Byte);
	void ins_ply(Byte);
	
	// 6502 instructions with new modes/flags on 65C02
	void ins_adc(Byte);
	void ins_and(Byte);
	void ins_asl(Byte);
	void ins_bit(Byte);
	void ins_brk(Byte);
	void ins_cmp(Byte);
	void ins_dec(Byte);
	void ins_eor(Byte);
	void ins_inc(Byte);
	void ins_jmp(Byte);
	void ins_lda(Byte);
	void ins_lsr(Byte);
	void ins_ora(Byte);
	void ins_ror(Byte);
	void ins_sbc(Byte);
	void ins_sta(Byte);
	
	// Instructions only available on the Rockwell variants of the 65C02 (R65C02).
	// These are assumed by the extended tests.
	void ins_bbr(Byte);
	void ins_bbs(Byte);
	void ins_rmb(Byte);
	void ins_smb(Byte);

	MOS6502::_instructionMap_t setup65C02Instructions(); 
}; // class 65C02
