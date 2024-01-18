#include <6502.h>

class MOS65C02 : public MOS6502 {
public:
    MOS65C02(Memory<Address_t, Byte>& m) : MOS6502(m)  {
		_instructions = setup65C02Instructions();
	}

    class OpcodeConstants : public MOS6502::OpcodeConstants  {
    public:
       	static constexpr Byte TSB_ZPI = 0x04;
        static constexpr Byte ASL_ZP  = 0x06;
        static constexpr Byte TSB_AII = 0x0c;
        static constexpr Byte ASL_ABS = 0x0e;
        static constexpr Byte ORA_ZPI = 0x12;
        static constexpr Byte TRB_ZPI = 0x14;
        static constexpr Byte ASL_ZPX = 0x16;
        static constexpr Byte INC_ACC = 0x1a;
        static constexpr Byte TRB_AII = 0x1c;
        static constexpr Byte ASL_ABX = 0x1e;
        static constexpr Byte AND_ZPI = 0x32;
        static constexpr Byte BIT_ZPX = 0x34;
        static constexpr Byte DEC_ACC = 0x3a;
        static constexpr Byte BIT_ABX = 0x3c;
        static constexpr Byte ROL_ABX = 0x3e;
        static constexpr Byte JMP_ABS = 0x4c;
        static constexpr Byte EOR_ZPI = 0x52;
        static constexpr Byte PHY_IMM = 0x5a;
        static constexpr Byte LSR_ABX = 0x5e;
        static constexpr Byte ADC_IDX = 0x61;
        static constexpr Byte STZ_ZPI = 0x64;
        static constexpr Byte ADC_ZP  = 0x65;
        static constexpr Byte ADC_IMM = 0x69;
        static constexpr Byte JMP_IND = 0x6c;
        static constexpr Byte ADC_ABS = 0x6d;
        static constexpr Byte ADC_IDY = 0x71;
        static constexpr Byte ADC_ZPI = 0x72;
        static constexpr Byte STZ_ZPX = 0x74;
        static constexpr Byte ADC_ZPX = 0x75;
        static constexpr Byte ADC_ABY = 0x79;
        static constexpr Byte PLY_IMM = 0x7a;
        static constexpr Byte JMP_ABX = 0x7c;
        static constexpr Byte ADC_ABX = 0x7d;
        static constexpr Byte ROR_ABX = 0x7e;
        static constexpr Byte BRA_REL = 0x80;
        static constexpr Byte BIT_IMM = 0x89;
        static constexpr Byte STA_ZPI = 0x92;
        static constexpr Byte STZ_AII = 0x9c;
        static constexpr Byte STZ_ABX = 0x9e;
        static constexpr Byte LDA_ZPI = 0xb2;
        static constexpr Byte CMP_ZPI = 0xd2;
        static constexpr Byte PHX_IMM = 0xda;
        static constexpr Byte SBC_IDX = 0xe1;
        static constexpr Byte SBC_ZP  = 0xe5;
        static constexpr Byte SBC_IMM = 0xe9;
        static constexpr Byte SBC_ABS = 0xed;
        static constexpr Byte SBC_IDY = 0xf1;
        static constexpr Byte SBC_ZPI = 0xf2;
        static constexpr Byte SBC_ZPX = 0xf5;
        static constexpr Byte SBC_ABY = 0xf9;
        static constexpr Byte PLX_IMM = 0xfa;
        static constexpr Byte SBC_ABX = 0xfd;
        static constexpr Byte INC_ABX = 0xfe;
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
			checkPageBoundary(opcode, address, X);
			address += X;
			Cycles++;
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
	}

	void ins_trb([[maybe_unused]] Byte opcode, [[maybe_unused]] Cycles_t& expectedCycles) {
		Word address = getAddress(opcode, expectedCycles);
		Byte data = readByte(address);
		Byte result = data & ~A;
		writeByte(address, result);
		setFlagZByValue(result);
		Cycles++;
	}

	void ins_tsb([[maybe_unused]] Byte opcode, [[maybe_unused]] Cycles_t& expectedCycles) {
		Word address = getAddress(opcode, expectedCycles);
		Byte data = readByte(address);
		Byte result = data | A;
		writeByte(address, result);
		setFlagZByValue(result);
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
		setFlagNByValue(Y);
		setFlagZByValue(Y);
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
		MOS6502::ins_bit(opcode, expectedCycles);
	}

	void ins_cmp(Byte opcode, Cycles_t& expectedCycles) {
		MOS6502::ins_cmp(opcode, expectedCycles);
	}

	void ins_dec(Byte opcode, Cycles_t& expectedCycles) {
		MOS6502::ins_dec(opcode, expectedCycles);
	}
	
	void ins_eor(Byte opcode, Cycles_t& expectedCycles) {
		MOS6502::ins_eor(opcode, expectedCycles);
	}

	void ins_inc(Byte opcode, Cycles_t& expectedCycles) {
		MOS6502::ins_inc(opcode, expectedCycles);
	}

	void ins_jmp([[maybe_unused]] Byte opcode, [[maybe_unused]] Cycles_t& expectedCycles) {
		// 65C02 JMP fixes the 6502 JMP bug
		Word address = readWord(PC);
		bool indirect = _instructions.at(opcode).addrmode == convertAddressingMode(AddressingMode::Indirect);
	
		if (indirect) {
			address = readWord(address);
			Cycles++;
			expectedCycles++;
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

	_instructionMap_t setup65C02Instructions() {

		_instructionMap_t newInstructions = {
		  // The table below is formatted as follows:
		  // { Opcode,
		  //   {"name", AddressingMode, ByteLength, CyclesUsed, Flags, Function pointer for instruction}}
		{ Opcodes.TSB_ZPI,
		  { "tsb", convertAddressingMode(AddressingMode::ZeroPageIndirect), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_tsb, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.TSB_AII,
		  { "tsb", convertAddressingMode(AddressingMode::AbsoluteIndexedIndirect), 3, 6, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_tsb, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.ORA_ZPI,
		  { "ora", convertAddressingMode(AddressingMode::ZeroPageIndirect), 2, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_ora, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.TRB_ZPI,
		  { "trb", convertAddressingMode(AddressingMode::ZeroPageIndirect), 3, 5, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_trb, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.INC_ACC,
		  { "inc", convertAddressingMode(AddressingMode::Accumulator), 1, 2, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_inc, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.TRB_AII,
		  { "trb", convertAddressingMode(AddressingMode::AbsoluteIndexedIndirect), 3, 6, InstructionFlags::None,
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
		{ Opcodes.PHY_IMM,
		  { "phy", convertAddressingMode(AddressingMode::Immediate), 1, 3, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_phy, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.LSR_ABX,
		  { "lsr", convertAddressingMode(AddressingMode::AbsoluteX), 3, 7, InstructionFlags::NoBoundaryCrossed,
		    std::bind(&MOS65C02::ins_lsr, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.ADC_IDX,
		  { "adc", convertAddressingMode(AddressingMode::IndirectX), 2, 6, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_adc, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.STZ_ZPI,
		  { "stz", convertAddressingMode(AddressingMode::ZeroPageIndirect), 2, 3, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_stz, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.ADC_ZP,
		  { "adc", convertAddressingMode(AddressingMode::ZeroPage), 2, 3, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_adc, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.ADC_IMM,
		  { "adc", convertAddressingMode(AddressingMode::Immediate), 2, 2, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_adc, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.JMP_IND,
		  { "jmp", convertAddressingMode(AddressingMode::Indirect), 3, 5, InstructionFlags::None,
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
		{ Opcodes.PLY_IMM,
		  { "ply", convertAddressingMode(AddressingMode::Immediate), 1, 4, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_ply, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.JMP_ABX,
		  { "jmp", convertAddressingMode(AddressingMode::AbsoluteX), 3, 6, InstructionFlags::None,
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
		{ Opcodes.STZ_AII,
		  { "stz", convertAddressingMode(AddressingMode::AbsoluteIndexedIndirect), 3, 4, InstructionFlags::None,
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
		{ Opcodes.PHX_IMM,
		  { "phx", convertAddressingMode(AddressingMode::Immediate), 1, 3, InstructionFlags::None,
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
		{ Opcodes.PLX_IMM,
		  { "plx", convertAddressingMode(AddressingMode::Immediate), 1, 4, InstructionFlags::None,
		    std::bind(&MOS65C02::ins_plx, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.SBC_ABX,
		  { "sbc", convertAddressingMode(AddressingMode::AbsoluteX), 3, 4, InstructionFlags::PageBoundary,
		    std::bind(&MOS65C02::ins_sbc, this, std::placeholders::_1, std::placeholders::_2)}},
		{ Opcodes.INC_ABX,
		  { "inc", convertAddressingMode(AddressingMode::AbsoluteX), 3, 7, InstructionFlags::NoBoundaryCrossed,
		    std::bind(&MOS65C02::ins_inc, this, std::placeholders::_1, std::placeholders::_2)}},
	};
	
		// Fold the new instructions into the 6502 instruction map
		auto instructions = MOS6502::setupInstructionMap();
		for (const auto& pair : newInstructions) {
			instructions[pair.first] = pair.second;
		}
		return instructions;
    }	
	
}; // class 65C02
