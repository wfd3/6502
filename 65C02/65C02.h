#include <6502.h>

// TODO: Remove when tests are complete
#include <fmt/format.h>
#include <fmt/core.h>

class MOS65C02 : public MOS6502 {
public:
    MOS65C02(Memory<Address_t, Byte>& m) : MOS6502(m)  {
		_instructions = setup65C02Instructions();
	}

    class OpcodeConstants : public MOS6502::OpcodeConstants  {
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
	
	MOS6502::AddressingMode convertAddressingMode(AddressingMode mode) {
        return static_cast<MOS6502::AddressingMode>(mode);
    }

	bool isAddrMode(Byte opcode, AddressingMode a) {
		return static_cast<MOS65C02::AddressingMode>(_instructions.at(opcode).addrmode) == a;
	}
	
	Word getAddress(Byte opcode, Cycles_t& expectedCycles) {
		Word address;

		auto checkPageBoundary = [&](Byte op, Word addr, Byte reg) {
			if ((_instructions.at(op).flags & InstructionFlags::NoBoundaryCrossed) &&
				((addr + reg) >> 8) == (addr >> 8)) {
				expectedCycles--;
				Cycles--;
			}
		};

		auto addressMode = static_cast<MOS65C02::AddressingMode>(_instructions.at(opcode).addrmode);
	
		switch(addressMode) {
		case AddressingMode::ZeroPageIndirect:
			address = static_cast<Byte>(readByteAtPC());
			address = readWord(address);
			break;
		
		case AddressingMode::AbsoluteIndexedIndirect:
			address = readWordAtPC();
			checkPageBoundary(opcode, address, X);
			address += X;
			Cycles++;
			break;

		default:
			address = MOS6502::getAddress(opcode, expectedCycles);
			break;
		}

		return address;
	}

	void decodeArgs(bool atPC, Byte ins, std::string& disassembly, std::string& opcodes, std::string& address, 
					std::string& computedAddr) {
		Byte byteval;
		Word wordval;
		std::string out, addr, label;

		// Check for Rockwell 65C02 BBRn or BBSn instructions. 
		if ((ins & 0x0f) == 0x0f || (ins & 0x07) == 0x07) {
			std::string zplabel, abslabel, zpaddr_str, reladdr_str, absaddr_str;
			// zpaddr, relative_address
			auto zpaddr  = readByteAtPC();
			auto reladdr = readByteAtPC();
			Word absaddr = PC + SByte(reladdr);

			zpaddr_str= fmt::format("${:02x}", zpaddr);
			zplabel = addressLabel(zpaddr);	

			if (!zplabel.empty()) {
				disassembly = zplabel;
				address = zpaddr;
			} else { 
				disassembly = zpaddr_str;
				address = "";
			} 

			reladdr_str = fmt::format("${:02x}", reladdr);
			abslabel = addressLabel(absaddr);
			absaddr_str = fmt::format("{:04x}", absaddr);

			if (!abslabel.empty()) {
				disassembly =+ "," + zplabel;
				address += "," + abslabel;
			} else { 
				disassembly += "," + reladdr_str;
				address = absaddr_str;
			} 

			opcodes += fmt::format("{:02x} {:02x} ", zpaddr, reladdr);
			return;
		}

		auto addressMode = static_cast<MOS65C02::AddressingMode>(_instructions.at(ins).addrmode);
		switch (addressMode) {
		case AddressingMode::ZeroPageIndirect:
			byteval = readByteAtPC();
			wordval = readWord(byteval);
			label = addressLabelSearch(wordval);
			addr = fmt::format("(${:02})", byteval);

			if (!label.empty()) {
				disassembly = label;
				address = addr;
			} else {
				disassembly = addr;
				address = "";
			}
			opcodes += fmt::format("{:02x} ", byteval);
			if (atPC) 
				computedAddr = fmt::format("{:02x}", byteval);
			break;

		case AddressingMode::AbsoluteIndexedIndirect:
			wordval = readWordAtPC();
			label = addressLabelSearch(wordval);
			addr = fmt::format("(${:04x}", wordval);
			if (!label.empty()) {
				disassembly = label;
				address = addr;
			} else {
				disassembly = addr;
				address = "";
			}
			disassembly += ",X";
			opcodes += fmt::format("{:02x} {:02x}", wordval & 0xff, (wordval >> 8) & 0xff);
			if (atPC) 
				computedAddr = fmt::format("${:04x}", wordval + X);

			break;

		default:
			MOS6502::decodeArgs(atPC, ins, disassembly, opcodes, address, computedAddr);
			break;
		}
	}



	// 65C02 specific instructions
	void ins_bra([[maybe_unused]] Byte opcode, [[maybe_unused]] Cycles_t& expectedCycles) {
		Word address = getAddress(opcode, expectedCycles);
		
		if ((PC >> 8) != (address >> 8)) { // Crossed page boundary
			Cycles++;
			expectedCycles++;
		}
		
		PC = address;
		Cycles++;
	}

	void ins_stz([[maybe_unused]] Byte opcode, [[maybe_unused]] Cycles_t& expectedCycles) {
		Address_t address = getAddress(opcode, expectedCycles);
		writeByte(address, 0);

		if (isAddrMode(opcode, AddressingMode::AbsoluteX))
			Cycles++;
	}

	void ins_trb([[maybe_unused]] Byte opcode, [[maybe_unused]] Cycles_t& expectedCycles) {
		Word address = getAddress(opcode, expectedCycles);
		Byte data = readByte(address);
		writeByte(address, data & ~A);
		setFlagZByValue(data & A);
		Cycles++;
	}

	void ins_tsb([[maybe_unused]] Byte opcode, [[maybe_unused]] Cycles_t& expectedCycles) {
		Word address = getAddress(opcode, expectedCycles);
		Byte data = readByte(address);
		writeByte(address, data | A);
		setFlagZByValue(data & A);
		Cycles++;
	}

	void ins_phx([[maybe_unused]] Byte opcode, [[maybe_unused]] Cycles_t& expectedCycles) {
		push(X);
		Cycles++;
	}

	void ins_phy([[maybe_unused]] Byte opcode, [[maybe_unused]] Cycles_t& expectedCycles) {
		push(Y);
		Cycles++;
	}

	void ins_plx([[maybe_unused]] Byte opcode, [[maybe_unused]] Cycles_t& expectedCycles) {
		X = pop();
		setFlagNByValue(X);
		setFlagZByValue(X);
		Cycles += 2;
	}
	void ins_ply([[maybe_unused]] Byte opcode, [[maybe_unused]] Cycles_t& expectedCycles) {
		Y = pop();
		setFlagNByValue(Y);
		setFlagZByValue(Y);
		Cycles += 2;
	}

	// 6502 instructions with new modes/flags on 65C02
	void ins_adc(Byte opcode, Cycles_t& expectedCycles) {
		MOS6502::ins_adc(opcode, expectedCycles);
		if (Flags.D) {
			Cycles++;
			expectedCycles++;
		}
	}

	void ins_and(Byte opcode, Cycles_t& expectedCycles) {
		MOS6502::ins_and(opcode, expectedCycles);
	}

	void ins_asl(Byte opcode, Cycles_t& expectedCycles) {
		MOS6502::ins_asl(opcode, expectedCycles);
	}

	void ins_bit(Byte opcode, Cycles_t& expectedCycles) {
		bool V, N;
		if (isAddrMode(opcode, AddressingMode::Immediate)) {
			V = Flags.V;
			N = Flags.N;
		}
		MOS6502::ins_bit(opcode, expectedCycles);
		if (isAddrMode(opcode, AddressingMode::Immediate)) {
			Flags.V = V;
			Flags.N = N;
		}
	}

	void ins_brk(Byte opcode, Cycles_t& expectedCycles) {
		MOS6502::ins_brk(opcode, expectedCycles);
		setFlagD(false);
	}

	void ins_cmp(Byte opcode, Cycles_t& expectedCycles) {
		MOS6502::ins_cmp(opcode, expectedCycles);
	}

	void ins_dec(Byte opcode, Cycles_t& expectedCycles) {
		bool accumulator = isAddrMode(opcode, AddressingMode::Accumulator);
		if (accumulator) {
			A--;
			Cycles++;
			setFlagZByValue(A);
			setFlagNByValue(A);
			return;
		} 

		MOS6502::ins_dec(opcode, expectedCycles);
	}
	
	void ins_eor(Byte opcode, Cycles_t& expectedCycles) {
		MOS6502::ins_eor(opcode, expectedCycles);
	}

	void ins_inc(Byte opcode, Cycles_t& expectedCycles) {
		bool accumulator = isAddrMode(opcode, AddressingMode::Accumulator);
		if (accumulator) {
			A++;
			Cycles++;
			setFlagZByValue(A);
			setFlagNByValue(A);
			return;
		} 

		MOS6502::ins_inc(opcode, expectedCycles);
	}

	// 65C02 JMP fixes the 6502 JMP bug and introduces a new addressing mode
	void ins_jmp([[maybe_unused]] Byte opcode, [[maybe_unused]] Cycles_t& expectedCycles) {
		Word address = readWord(PC);
		
		bool indirect = isAddrMode(opcode, AddressingMode::Indirect);
		bool absIndexedIndirect = isAddrMode(opcode, AddressingMode::AbsoluteIndexedIndirect);
	
		if (absIndexedIndirect) {
			address += X;
		}
		if (indirect || absIndexedIndirect) {
			address = readWord(address);
			Cycles++;
		}
		
		PC = address;
	}

	void ins_lda(Byte opcode, Cycles_t& expectedCycles) {
		MOS6502::ins_lda(opcode, expectedCycles);
	}
		
	void ins_lsr(Byte opcode, Cycles_t& expectedCycles) {
		MOS6502::ins_lsr(opcode, expectedCycles);
	}
	
	void ins_ora(Byte opcode, Cycles_t& expectedCycles) {
		MOS6502::ins_ora(opcode, expectedCycles);
	}

	void ins_ror(Byte opcode, Cycles_t& expectedCycles) {
		MOS6502::ins_ror(opcode, expectedCycles);
	}

	void ins_sbc(Byte opcode, Cycles_t& expectedCycles) {
		MOS6502::ins_sbc(opcode, expectedCycles);
		if (Flags.D) {
			Cycles++;
			expectedCycles++;
		}
	}
	
	void ins_sta(Byte opcode, Cycles_t& expectedCycles) {
		MOS6502::ins_sta(opcode, expectedCycles);
	}

	// Instructions only available on the Rockwell variants of the 65C02 (R65C02).
	// These are assumed by the extended opcode tests.

	// Branch on Bit Reset
	void ins_bbr(Byte opcode, Cycles_t& expectedCycles) {
		Byte zpaddr = readByteAtPC();
		Word address = getAddress(opcode, expectedCycles);
		Byte m = readByte(zpaddr);

		Byte bitmask = 1 << (opcode >> 4);
		bool bitset = bitmask & m;
		if (!bitset) 
			PC = address;
		Cycles++;
	}

	// Branch on Bit Set -- sets D flag??
	void ins_bbs(Byte opcode, Cycles_t& expectedCycles) {
		Byte zpaddr = readByteAtPC();
		Word address = getAddress(opcode, expectedCycles);
		Byte m = readByte(zpaddr);

		Byte bitmask = 1 << ((opcode >> 4) - 8);
		bool bitset = bitmask & m;
		if (bitset) 
			PC = address;
		Cycles++;
	}

	// Reset Memory Bit
	void ins_rmb(Byte opcode, [[maybe_unused]] Cycles_t& expectedCycles) {
		Byte zpaddr = readByteAtPC();
		Byte bitmask = 1 << ((opcode >> 4));
		Byte m = readByte(zpaddr);
		m = m & ~bitmask;
		writeByte(zpaddr, m);
		Cycles++;
	}

	// Set Memory Bit
	void ins_smb(Byte opcode, [[maybe_unused]] Cycles_t& expectedCycles) {
		Byte zpaddr = readByteAtPC();
		Byte bitmask = 1 << ((opcode >> 4) - 8);
		Byte m = readByte(zpaddr);
		m = m | bitmask;
		writeByte(zpaddr, m);
		Cycles++;
	}
	
	
	_instructionMap_t setup65C02Instructions() {

		_instructionMap_t newInstructions = {
		  // The table below is formatted as follows:
		  // { Opcode,
		  //   {"name", AddressingMode, ByteLength, CyclesUsed, Flags, Function pointer for instruction}}
		{ Opcodes.TSB_ZP,
		  { "tsb", convertAddressingMode(AddressingMode::ZeroPage), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_tsb, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.TSB_ABS,
		  { "tsb", convertAddressingMode(AddressingMode::Absolute), 3, 6, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_tsb, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.ORA_ZPI,
		  { "ora", convertAddressingMode(AddressingMode::ZeroPageIndirect), 2, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_ora, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.TRB_ZP,
		  { "trb", convertAddressingMode(AddressingMode::ZeroPage), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_trb, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.INC_ACC,
		  { "inc", convertAddressingMode(AddressingMode::Accumulator), 1, 2, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_inc, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.TRB_ABS,
		  { "trb", convertAddressingMode(AddressingMode::Absolute), 3, 6, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_trb, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.ASL_ABX,
		  { "asl", convertAddressingMode(AddressingMode::AbsoluteX), 3, 7, InstructionFlags::NoBoundaryCrossed,
		    std::bind(&MOS65C02::ins_asl, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.AND_ZPI,
		  { "and", convertAddressingMode(AddressingMode::ZeroPageIndirect), 2, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_and, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.BIT_ZPX,
		  { "bit", convertAddressingMode(AddressingMode::ZeroPageX), 2, 4, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_bit, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.DEC_ACC,
		  { "dec", convertAddressingMode(AddressingMode::Accumulator), 1, 2, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_dec, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.BIT_ABX,
		  { "bit", convertAddressingMode(AddressingMode::AbsoluteX), 3, 4, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_bit, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.ROL_ABX,
		  { "rol", convertAddressingMode(AddressingMode::AbsoluteX), 3, 7, InstructionFlags::NoBoundaryCrossed,
		    std::bind(&MOS65C02::ins_rol, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.JMP_ABS,
		  { "jmp", convertAddressingMode(AddressingMode::Absolute), 3, 3, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_jmp, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.EOR_ZPI,
		  { "eor", convertAddressingMode(AddressingMode::ZeroPageIndirect), 2, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_eor, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.PHY_IMP,
		  { "phy", convertAddressingMode(AddressingMode::Implied), 1, 3, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_phy, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.LSR_ABX,
		  { "lsr", convertAddressingMode(AddressingMode::AbsoluteX), 3, 7, InstructionFlags::NoBoundaryCrossed,
		    std::bind(&MOS65C02::ins_lsr, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.ADC_IDX,
		  { "adc", convertAddressingMode(AddressingMode::IndirectX), 2, 6, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_adc, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.STZ_ZP,
		  { "stz", convertAddressingMode(AddressingMode::ZeroPage), 2, 3, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_stz, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.ADC_ZP,
		  { "adc", convertAddressingMode(AddressingMode::ZeroPage), 2, 3, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_adc, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.ADC_IMM,
		  { "adc", convertAddressingMode(AddressingMode::Immediate), 2, 2, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_adc, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.JMP_IND,
		  { "jmp", convertAddressingMode(AddressingMode::Indirect), 3, 6, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_jmp, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.ADC_ABS,
		  { "adc", convertAddressingMode(AddressingMode::Absolute), 3, 4, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_adc, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.ADC_IDY,
		  { "adc", convertAddressingMode(AddressingMode::IndirectY), 2, 5, InstructionFlags::PageBoundary,
		    std::bind(&MOS65C02::ins_adc, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.ADC_ZPI,
		  { "adc", convertAddressingMode(AddressingMode::ZeroPageIndirect), 2, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_adc, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.STZ_ZPX,
		  { "stz", convertAddressingMode(AddressingMode::ZeroPageX), 2, 4, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_stz, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.ADC_ZPX,
		  { "adc", convertAddressingMode(AddressingMode::ZeroPageX), 2, 4, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_adc, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.ADC_ABY,
		  { "adc", convertAddressingMode(AddressingMode::AbsoluteY), 3, 4, InstructionFlags::PageBoundary,
		    std::bind(&MOS65C02::ins_adc, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.PLY_IMP,
		  { "ply", convertAddressingMode(AddressingMode::Implied), 1, 4, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_ply, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.JMP_AII,
		  { "jmp", convertAddressingMode(AddressingMode::AbsoluteIndexedIndirect), 3, 6, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_jmp, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.ADC_ABX,
		  { "adc", convertAddressingMode(AddressingMode::AbsoluteX), 3, 4, InstructionFlags::PageBoundary,
		    std::bind(&MOS65C02::ins_adc, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.ROR_ABX,
		  { "ror", convertAddressingMode(AddressingMode::AbsoluteX), 3, 7, InstructionFlags::NoBoundaryCrossed,
		    std::bind(&MOS65C02::ins_ror, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.BRA_REL,
		  { "bra", convertAddressingMode(AddressingMode::Relative), 2, 3, InstructionFlags::PageBoundary,
		    std::bind(&MOS65C02::ins_bra, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.BIT_IMM,
		  { "bit", convertAddressingMode(AddressingMode::Immediate), 2, 2, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_bit, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.STA_ZPI,
		  { "sta", convertAddressingMode(AddressingMode::ZeroPageIndirect), 2, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_sta, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.STZ_ABS,
		  { "stz", convertAddressingMode(AddressingMode::Absolute), 3, 4, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_stz, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.STZ_ABX,
		  { "stz", convertAddressingMode(AddressingMode::AbsoluteX), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_stz, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.LDA_ZPI,
		  { "lda", convertAddressingMode(AddressingMode::ZeroPageIndirect), 2, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_lda, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.CMP_ZPI,
		  { "cmp", convertAddressingMode(AddressingMode::ZeroPageIndirect), 2, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_cmp, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.PHX_IMP,
		  { "phx", convertAddressingMode(AddressingMode::Implied), 1, 3, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_phx, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.DEC_ABX,
		  { "dec", convertAddressingMode(AddressingMode::AbsoluteX), 3, 7, InstructionFlags::NoBoundaryCrossed,
		    std::bind(&MOS65C02::ins_dec, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.SBC_IDX,
		  { "sbc", convertAddressingMode(AddressingMode::IndirectX), 2, 6, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_sbc, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.SBC_ZP,
		  { "sbc", convertAddressingMode(AddressingMode::ZeroPage), 2, 3, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_sbc, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.SBC_IMM,
		  { "sbc", convertAddressingMode(AddressingMode::Immediate), 2, 2, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_sbc, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.SBC_ABS,
		  { "sbc", convertAddressingMode(AddressingMode::Absolute), 3, 4, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_sbc, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.SBC_IDY,
		  { "sbc", convertAddressingMode(AddressingMode::IndirectY), 2, 5, InstructionFlags::PageBoundary,
		    std::bind(&MOS65C02::ins_sbc, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.SBC_ZPI,
		  { "sbc", convertAddressingMode(AddressingMode::ZeroPageIndirect), 2, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_sbc, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.SBC_ZPX,
		  { "sbc", convertAddressingMode(AddressingMode::ZeroPageX), 2, 4, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_sbc, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.SBC_ABY,
		  { "sbc", convertAddressingMode(AddressingMode::AbsoluteY), 3, 4, InstructionFlags::PageBoundary,
		    std::bind(&MOS65C02::ins_sbc, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.PLX_IMP,
		  { "plx", convertAddressingMode(AddressingMode::Implied), 1, 4, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_plx, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.SBC_ABX,
		  { "sbc", convertAddressingMode(AddressingMode::AbsoluteX), 3, 4, InstructionFlags::PageBoundary,
		    std::bind(&MOS65C02::ins_sbc, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.INC_ABX,
		  { "inc", convertAddressingMode(AddressingMode::AbsoluteX), 3, 7, InstructionFlags::NoBoundaryCrossed,
		    std::bind(&MOS65C02::ins_inc, this, std::placeholders::_1, std::placeholders::_2)}},

		// R65C02 instructions
		{ Opcodes.BBR0,
		  { "bbr0", convertAddressingMode(AddressingMode::Relative), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_bbr, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.BBR1,
		  { "bbr1", convertAddressingMode(AddressingMode::Relative), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_bbr, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.BBR2,
		  { "bbr2", convertAddressingMode(AddressingMode::Relative), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_bbr, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.BBR3,
		  { "bbr3", convertAddressingMode(AddressingMode::Relative), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_bbr, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.BBR4,
		  { "bbr4", convertAddressingMode(AddressingMode::Relative), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_bbr, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.BBR5,
		  { "bbr5", convertAddressingMode(AddressingMode::Relative), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_bbr, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.BBR6,
		  { "bbr6", convertAddressingMode(AddressingMode::Relative), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_bbr, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.BBR7,
		  { "bbr7", convertAddressingMode(AddressingMode::Relative), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_bbr, this, std::placeholders::_1, std::placeholders::_2)}},

		{ Opcodes.BBS0,
		  { "bbs0", convertAddressingMode(AddressingMode::Relative), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_bbs, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.BBS1,
		  { "bbs1", convertAddressingMode(AddressingMode::Relative), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_bbs, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.BBS2,
		  { "bbs2", convertAddressingMode(AddressingMode::Relative), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_bbs, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.BBS3,
		  { "bbs3", convertAddressingMode(AddressingMode::Relative), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_bbs, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.BBS4,
		  { "bbs4", convertAddressingMode(AddressingMode::Relative), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_bbs, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.BBS5,
		  { "bbs5", convertAddressingMode(AddressingMode::Relative), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_bbs, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.BBS6,
		  { "bbs6", convertAddressingMode(AddressingMode::Relative), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_bbs, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.BBS7,
		  { "bbs7", convertAddressingMode(AddressingMode::Relative), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_bbs, this, std::placeholders::_1, std::placeholders::_2)}},

		{ Opcodes.RMB0,
		  { "rmb0", convertAddressingMode(AddressingMode::ZeroPage), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_rmb, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.RMB1,
		  { "rmb1", convertAddressingMode(AddressingMode::ZeroPage), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_rmb, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.RMB2,
		  { "rmb2", convertAddressingMode(AddressingMode::ZeroPage), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_rmb, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.RMB3,
		  { "rmb3", convertAddressingMode(AddressingMode::ZeroPage), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_rmb, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.RMB4,
		  { "rmb4", convertAddressingMode(AddressingMode::ZeroPage), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_rmb, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.RMB5,
		  { "rmb5", convertAddressingMode(AddressingMode::ZeroPage), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_rmb, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.RMB6,
		  { "rmb6", convertAddressingMode(AddressingMode::ZeroPage), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_rmb, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.RMB7,
		  { "rmb7", convertAddressingMode(AddressingMode::ZeroPage), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_rmb, this, std::placeholders::_1, std::placeholders::_2)}},

		{ Opcodes.SMB0,
		  { "smb0", convertAddressingMode(AddressingMode::ZeroPage), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_smb, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.SMB1,
		  { "smb1", convertAddressingMode(AddressingMode::ZeroPage), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_smb, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.SMB2,
		  { "smb2", convertAddressingMode(AddressingMode::ZeroPage), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_smb, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.SMB3,
		  { "smb3", convertAddressingMode(AddressingMode::ZeroPage), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_smb, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.SMB4,
		  { "smb4", convertAddressingMode(AddressingMode::ZeroPage), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_smb, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.SMB5,
		  { "smb5", convertAddressingMode(AddressingMode::ZeroPage), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_smb, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.SMB6,
		  { "smb6", convertAddressingMode(AddressingMode::ZeroPage), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_smb, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.SMB7,
		  { "smb7", convertAddressingMode(AddressingMode::ZeroPage), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_smb, this, std::placeholders::_1, std::placeholders::_2)}},

		// 65C02 BRK resets D flag
		{ Opcodes.BRK_IMM,
		  { "brk", convertAddressingMode(AddressingMode::Immediate), 1, 7, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_brk, this, std::placeholders::_1, std::placeholders::_2)}},

	};
	
		// Fold the new instructions into the 6502 instruction map
		auto instructions = MOS6502::setupInstructionMap();
		for (const auto& pair : newInstructions) {
			instructions[pair.first] = pair.second;
		}
		return instructions;
    }	
	
}; // class 65C02
