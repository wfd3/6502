	#include <gtest/gtest.h>
	#include <6502.h>

	class MOS6502CMPTests : public testing::Test {
	public:

		Memory<Address_t, Byte> mem{CPU::MAX_MEM};
		CPU cpu{mem};

		virtual void SetUp() {
			mem.mapRAM(0, CPU::MAX_MEM);
		}
		
		virtual void TearDown()	{
		}
	};

	TEST_F(MOS6502CMPTests, CmpImmediate) {
		uint64_t UsedCycles, ExpectedCycles;
		Byte ins = CPU::INS_CMP_IMM;

		//Given:
		cpu.TestReset(CPU::RESET_VECTOR);
		
		mem[0xFFFC] = ins;
		mem[0xFFFD] = 0x20;
		cpu.A = 0x30;

		//When:
		std::tie(UsedCycles, ExpectedCycles) =
			cpu.executeOneInstruction();

		// Then:
		EXPECT_TRUE(cpu.Flags.C);
		EXPECT_FALSE(cpu.Flags.Z);
		EXPECT_FALSE(cpu.Flags.N);
		EXPECT_EQ(UsedCycles, ExpectedCycles); 
	}

	TEST_F(MOS6502CMPTests, CmpImmediateSetsCFlagFalse) {
		uint64_t UsedCycles, ExpectedCycles;
		Byte ins = CPU::INS_CMP_IMM;

		//Given:
			cpu.TestReset(CPU::RESET_VECTOR);	

		mem[0xFFFC] = ins;
		mem[0xFFFD] = 0xFF;
		cpu.A = 0x0F;

		//When:
		std::tie(UsedCycles, ExpectedCycles) =
			cpu.executeOneInstruction();

		// Then:
		EXPECT_FALSE(cpu.Flags.C);
		EXPECT_FALSE(cpu.Flags.Z);
		EXPECT_FALSE(cpu.Flags.N);
		EXPECT_EQ(UsedCycles, ExpectedCycles); 
	}

	TEST_F(MOS6502CMPTests, CmpImmediateSetsZFlagTrue) {
		uint64_t UsedCycles, ExpectedCycles;
		Byte ins = CPU::INS_CMP_IMM;

		//Given:
			cpu.TestReset(CPU::RESET_VECTOR);	

		mem[0xFFFC] = ins;
		mem[0xFFFD] = 0xFF;
		cpu.A = 0xFF;

		//When:
		std::tie(UsedCycles, ExpectedCycles) =
			cpu.executeOneInstruction();

		// Then:
		EXPECT_TRUE(cpu.Flags.C);
		EXPECT_TRUE(cpu.Flags.Z);
		EXPECT_FALSE(cpu.Flags.N);
		EXPECT_EQ(UsedCycles, ExpectedCycles); 
	}

	TEST_F(MOS6502CMPTests, CmpImmediateSetsNFlagTrue) {
		uint64_t UsedCycles, ExpectedCycles;
		Byte ins = CPU::INS_CMP_IMM;

		//Given:
		cpu.TestReset(CPU::RESET_VECTOR);	

		mem[0xFFFC] = ins;
		mem[0xFFFD] = 0x20;
		cpu.A = 0x10;

		//When:
		std::tie(UsedCycles, ExpectedCycles) =
			cpu.executeOneInstruction();

		// Then:
		EXPECT_FALSE(cpu.Flags.C);
		EXPECT_FALSE(cpu.Flags.Z);
		EXPECT_TRUE(cpu.Flags.N);
		EXPECT_EQ(UsedCycles, ExpectedCycles); 
	}

	TEST_F(MOS6502CMPTests, CmpZeroPage) {
		uint64_t UsedCycles, ExpectedCycles;
		Byte ins = CPU::INS_CMP_ZP;

		//Given:
		cpu.TestReset(CPU::RESET_VECTOR);	

		mem[0xFFFC] = ins;
		mem[0xFFFD] = 0x00;
		mem[0x0000] = 0x0F;
		cpu.A = 0x20;

		//When:
		std::tie(UsedCycles, ExpectedCycles) =
			cpu.executeOneInstruction();

		// Then:
		EXPECT_TRUE(cpu.Flags.C);
		EXPECT_FALSE(cpu.Flags.Z);
		EXPECT_FALSE(cpu.Flags.N);
		EXPECT_EQ(UsedCycles, ExpectedCycles); 
	}

	TEST_F(MOS6502CMPTests, CmpZeroPageX) {
		uint64_t UsedCycles, ExpectedCycles;
		Byte ins = CPU::INS_CMP_ZPX;

		//Given:
		cpu.TestReset(CPU::RESET_VECTOR);	

		mem[0xFFFC] = ins;
		mem[0xFFFD] = 0x00;
		cpu.X = 0x01;
		mem[0x0001] = 0x0F;
		cpu.A = 0x2F;

		//When:
		std::tie(UsedCycles, ExpectedCycles) =
			cpu.executeOneInstruction();

		// Then:
		EXPECT_TRUE(cpu.Flags.C);
		EXPECT_FALSE(cpu.Flags.Z);
		EXPECT_FALSE(cpu.Flags.N);
		EXPECT_EQ(UsedCycles, ExpectedCycles); 
	}

	TEST_F(MOS6502CMPTests, CmpAbsolute) {
		uint64_t UsedCycles, ExpectedCycles;
		Byte ins = CPU::INS_CMP_ABS;

		//Given:
		cpu.TestReset(CPU::RESET_VECTOR);	

		mem[0xFFFC] = ins;
		mem[0xFFFD] = 0x00;
		mem[0xFFFE] = 0x20;
		mem[0x2000] = 0x0F;
		cpu.A = 0x2F;

		//When:
		std::tie(UsedCycles, ExpectedCycles) =
			cpu.executeOneInstruction();

		// Then:
		EXPECT_TRUE(cpu.Flags.C);
		EXPECT_FALSE(cpu.Flags.Z);
		EXPECT_FALSE(cpu.Flags.N);
		EXPECT_EQ(UsedCycles, ExpectedCycles); 
	}

	TEST_F(MOS6502CMPTests, CmpAbsoluteX) {
		uint64_t UsedCycles, ExpectedCycles;
		Byte ins = CPU::INS_CMP_ABX;

		//Given:
		cpu.TestReset(CPU::RESET_VECTOR);	

		mem[0xFFFC] = ins;
		mem[0xFFFD] = 0x00;
		mem[0xFFFE] = 0x20;
		cpu.X = 0x01;
		mem[0x2001] = 0x0F;
		cpu.A = 0x2F;

		//When:
		std::tie(UsedCycles, ExpectedCycles) =
			cpu.executeOneInstruction();

		// Then:
		EXPECT_TRUE(cpu.Flags.C);
		EXPECT_FALSE(cpu.Flags.Z);
		EXPECT_FALSE(cpu.Flags.N);
		EXPECT_EQ(UsedCycles, ExpectedCycles); 
	}

	TEST_F(MOS6502CMPTests, CmpAbsoluteXCrossesPage) {
		uint64_t UsedCycles, ExpectedCycles;
		Byte ins = CPU::INS_CMP_ABX;

		//Given:
		cpu.TestReset(CPU::RESET_VECTOR);	

		mem[0xFFFC] = ins;
		mem[0xFFFD] = 0x10;
		mem[0xFFFE] = 0x20;
		cpu.X = 0xFF;
		mem[0x210f] = 0x0F;
		cpu.A = 0x2F;

		//When:
		std::tie(UsedCycles, ExpectedCycles) =
			cpu.executeOneInstruction();

		// Then:
		EXPECT_TRUE(cpu.Flags.C);
		EXPECT_FALSE(cpu.Flags.Z);
		EXPECT_FALSE(cpu.Flags.N);
		EXPECT_EQ(UsedCycles, ExpectedCycles); 
	}

	TEST_F(MOS6502CMPTests, CmpAbsoluteY) {
		uint64_t UsedCycles, ExpectedCycles;
		Byte ins = CPU::INS_CMP_ABY;

		//Given:
		cpu.TestReset(CPU::RESET_VECTOR);	

		mem[0xFFFC] = ins;
		mem[0xFFFD] = 0x00;
		mem[0xFFFE] = 0x20;
		cpu.Y = 0x01;
		mem[0x2001] = 0x0F;
		cpu.A = 0x2F;

		//When:
		std::tie(UsedCycles, ExpectedCycles) =
			cpu.executeOneInstruction();

		// Then:
		EXPECT_TRUE(cpu.Flags.C);
		EXPECT_FALSE(cpu.Flags.Z);
		EXPECT_FALSE(cpu.Flags.N);
		EXPECT_EQ(UsedCycles, ExpectedCycles); 
	}

	TEST_F(MOS6502CMPTests, CmpAbsoluteYCrossesPage) {
		uint64_t UsedCycles, ExpectedCycles;
		Byte ins = CPU::INS_CMP_ABY;

		//Given:
		cpu.TestReset(CPU::RESET_VECTOR);	

		mem[0xFFFC] = ins;
		mem[0xFFFD] = 0x10;
		mem[0xFFFE] = 0x20;
		cpu.Y = 0xFF;
		mem[0x210f] = 0x0F;
		cpu.A = 0x2F;

		//When:
		std::tie(UsedCycles, ExpectedCycles) =
			cpu.executeOneInstruction();

		// Then:
		EXPECT_TRUE(cpu.Flags.C);
		EXPECT_FALSE(cpu.Flags.Z);
		EXPECT_FALSE(cpu.Flags.N);
		EXPECT_EQ(UsedCycles, ExpectedCycles); 
	}

	TEST_F(MOS6502CMPTests, CmpIndirectX) {
		uint64_t UsedCycles, ExpectedCycles;
		Byte ins = CPU::INS_CMP_IDX;

		//Given:
		cpu.TestReset(CPU::RESET_VECTOR);	

		mem[0xFFFC] = ins;
		mem[0xFFFD] = 0x01;
		cpu.X = 0x01;
		mem[0x0002] = 0x01;
		mem[0x0000] = 0x20;
		mem[0x2001] = 0x0F;
		cpu.A = 0x2F;

		//When:
		std::tie(UsedCycles, ExpectedCycles) =
			cpu.executeOneInstruction();

		// Then:
		EXPECT_TRUE(cpu.Flags.C);
		EXPECT_FALSE(cpu.Flags.Z);
		EXPECT_FALSE(cpu.Flags.N);
		EXPECT_EQ(UsedCycles, ExpectedCycles); 
	}

	TEST_F(MOS6502CMPTests, CmpIndirectY) {
		uint64_t UsedCycles, ExpectedCycles;
		Byte ins = CPU::INS_CMP_IDY;

		//Given:
		cpu.TestReset(CPU::RESET_VECTOR);	

		mem[0xFFFC] = ins;
		mem[0xFFFD] = 0x02;
		cpu.Y = 0x01;
		mem[0x0002] = 0x01;
		mem[0x0000] = 0x20;
		mem[0x2002] = 0x0F;
		cpu.A = 0x2F;

		//When:
		std::tie(UsedCycles, ExpectedCycles) =
			cpu.executeOneInstruction();

		// Then:
		EXPECT_TRUE(cpu.Flags.C);
		EXPECT_FALSE(cpu.Flags.Z);
		EXPECT_FALSE(cpu.Flags.N);
		EXPECT_EQ(UsedCycles, ExpectedCycles); 
	}

	TEST_F(MOS6502CMPTests, CmpIndirectYCrossesPage) {
		uint64_t UsedCycles, ExpectedCycles;
		Byte ins = CPU::INS_CMP_IDY;

		//Given:
		cpu.TestReset(CPU::RESET_VECTOR);	

		mem[0xFFFC] = ins;
		mem[0xFFFD] = 0x02;
		cpu.Y = 0xFF;
		mem[0x0002] = 0x10;
		mem[0x0000] = 0x20;
		mem[0x210F] = 0x0F;
		cpu.A = 0x2F;

		//When:
		std::tie(UsedCycles, ExpectedCycles) =
			cpu.executeOneInstruction();

		// Then:
		EXPECT_TRUE(cpu.Flags.C);
		EXPECT_FALSE(cpu.Flags.Z);
		EXPECT_FALSE(cpu.Flags.N);
		EXPECT_EQ(UsedCycles, ExpectedCycles); 
	}

	// CPX
	TEST_F(MOS6502CMPTests, CpxImmediate) {
		uint64_t UsedCycles, ExpectedCycles;
		Byte ins = CPU::INS_CPX_IMM;

		//Given:
		cpu.TestReset(CPU::RESET_VECTOR);	

		mem[0xFFFC] = ins;
		mem[0xFFFD] = 0x20;
		cpu.X = 0x30;

		//When:
		std::tie(UsedCycles, ExpectedCycles) =
			cpu.executeOneInstruction();

		// Then:
		EXPECT_TRUE(cpu.Flags.C);
		EXPECT_FALSE(cpu.Flags.Z);
		EXPECT_FALSE(cpu.Flags.N);
		EXPECT_EQ(UsedCycles, ExpectedCycles); 
	}

	TEST_F(MOS6502CMPTests, CpxImmediateSetsCFlagFalse) {
		uint64_t UsedCycles, ExpectedCycles;
		Byte ins = CPU::INS_CPX_IMM;

		//Given:
		cpu.TestReset(CPU::RESET_VECTOR);	

		mem[0xFFFC] = ins;
		mem[0xFFFD] = 0xf;
		cpu.X = 0x0e;

		//When:
		std::tie(UsedCycles, ExpectedCycles) =
			cpu.executeOneInstruction();

		// Then:
		EXPECT_FALSE(cpu.Flags.C);
		EXPECT_FALSE(cpu.Flags.Z);
		EXPECT_TRUE(cpu.Flags.N);
		EXPECT_EQ(UsedCycles, ExpectedCycles); 
	}

	TEST_F(MOS6502CMPTests, CpxImmediateSetsZFlagTrue) {
		uint64_t UsedCycles, ExpectedCycles;
		Byte ins = CPU::INS_CPX_IMM;

		//Given:
		cpu.TestReset(CPU::RESET_VECTOR);	

		mem[0xFFFC] = ins;
		mem[0xFFFD] = 0xFF;
		cpu.X = 0xFF;

		//When:
		std::tie(UsedCycles, ExpectedCycles) =
			cpu.executeOneInstruction();

		// Then:
		EXPECT_TRUE(cpu.Flags.C);
		EXPECT_TRUE(cpu.Flags.Z);
		EXPECT_FALSE(cpu.Flags.N);
		EXPECT_EQ(UsedCycles, ExpectedCycles); 
	}

	TEST_F(MOS6502CMPTests, CpxImmediateSetsNFlagTrue) {
		uint64_t UsedCycles, ExpectedCycles;
		Byte ins = CPU::INS_CPX_IMM;

		//Given:
		cpu.TestReset(CPU::RESET_VECTOR);	

		mem[0xFFFC] = ins;
		mem[0xFFFD] = 0x2F;
		cpu.X = 0x1F;

		//When:
		std::tie(UsedCycles, ExpectedCycles) =
			cpu.executeOneInstruction();

		// Then:
		EXPECT_FALSE(cpu.Flags.C);
		EXPECT_FALSE(cpu.Flags.Z);
		EXPECT_TRUE(cpu.Flags.N);
		EXPECT_EQ(UsedCycles, ExpectedCycles); 
	}

	TEST_F(MOS6502CMPTests, CpxZeroPage) {
		uint64_t UsedCycles, ExpectedCycles;
		Byte ins = CPU::INS_CPX_ZP;

		//Given:
		cpu.TestReset(CPU::RESET_VECTOR);	

		mem[0xFFFC] = ins;
		mem[0xFFFD] = 0x00;
		mem[0x0000] = 0x0F;
		cpu.X = 0x2F;

		//When:
		std::tie(UsedCycles, ExpectedCycles) =
			cpu.executeOneInstruction();

		// Then:
		EXPECT_TRUE(cpu.Flags.C);
		EXPECT_FALSE(cpu.Flags.Z);
		EXPECT_FALSE(cpu.Flags.N);
		EXPECT_EQ(UsedCycles, ExpectedCycles); 
	}

	TEST_F(MOS6502CMPTests, CpxAbsolute) {
		uint64_t UsedCycles, ExpectedCycles;
		Byte ins = CPU::INS_CPX_ABS;

		//Given:
		cpu.TestReset(CPU::RESET_VECTOR);	

		mem[0xFFFC] = ins;
		mem[0xFFFD] = 0x00;
		mem[0xFFFE] = 0x20;
		mem[0x2000] = 0x0F;
		cpu.X = 0x2F;

		//When:
		std::tie(UsedCycles, ExpectedCycles) =
			cpu.executeOneInstruction();

		// Then:
		EXPECT_TRUE(cpu.Flags.C);
		EXPECT_FALSE(cpu.Flags.Z);
		EXPECT_FALSE(cpu.Flags.N);
		EXPECT_EQ(UsedCycles, ExpectedCycles); 
	}

	// CPY
	TEST_F(MOS6502CMPTests, CpyImmediate) {
		uint64_t UsedCycles, ExpectedCycles;
		Byte ins = CPU::INS_CPY_IMM;

		//Given:
		cpu.TestReset(CPU::RESET_VECTOR);	

		mem[0xFFFC] = ins;
		mem[0xFFFD] = 0x20;
		cpu.Y = 0x30;

		//When:
		std::tie(UsedCycles, ExpectedCycles) =
			cpu.executeOneInstruction();

		// Then:
		EXPECT_TRUE(cpu.Flags.C);
		EXPECT_FALSE(cpu.Flags.Z);
		EXPECT_FALSE(cpu.Flags.N);
		EXPECT_EQ(UsedCycles, ExpectedCycles); 
	}

	TEST_F(MOS6502CMPTests, CpyImmediateSetsCFlagFalse) {
		uint64_t UsedCycles, ExpectedCycles;
		Byte ins = CPU::INS_CPY_IMM;

		//Given:
		cpu.TestReset(CPU::RESET_VECTOR);	

		mem[0xFFFC] = ins;
		mem[0xFFFD] = 0xf;
		cpu.Y = 0xe;

		//When:
		std::tie(UsedCycles, ExpectedCycles) =
			cpu.executeOneInstruction();

		// Then:
		EXPECT_FALSE(cpu.Flags.C);
		EXPECT_FALSE(cpu.Flags.Z);
		EXPECT_TRUE(cpu.Flags.N);
		EXPECT_EQ(UsedCycles, ExpectedCycles); 
	}

	TEST_F(MOS6502CMPTests, CpyImmediateSetsZFlagTrue) {
		uint64_t UsedCycles, ExpectedCycles;
		Byte ins = CPU::INS_CPY_IMM;

		//Given:
		cpu.TestReset(CPU::RESET_VECTOR);	

		mem[0xFFFC] = ins;
		mem[0xFFFD] = 0xFF;
		cpu.Y = 0xFF;

		//When:
		std::tie(UsedCycles, ExpectedCycles) =
			cpu.executeOneInstruction();

		// Then:
		EXPECT_TRUE(cpu.Flags.C);
		EXPECT_TRUE(cpu.Flags.Z);
		EXPECT_FALSE(cpu.Flags.N);
		EXPECT_EQ(UsedCycles, ExpectedCycles); 
	}

	TEST_F(MOS6502CMPTests, CpyImmediateSetsNFlagTrue) {
		uint64_t UsedCycles, ExpectedCycles;
		Byte ins = CPU::INS_CPY_IMM;

		//Given:
		cpu.TestReset(CPU::RESET_VECTOR);	

		mem[0xFFFC] = ins;
		mem[0xFFFD] = 0x3F;
		cpu.Y = 0x2F;

		//When:
		std::tie(UsedCycles, ExpectedCycles) =
			cpu.executeOneInstruction();

		// Then:
		EXPECT_FALSE(cpu.Flags.C);
		EXPECT_FALSE(cpu.Flags.Z);
		EXPECT_TRUE(cpu.Flags.N);
		EXPECT_EQ(UsedCycles, ExpectedCycles); 
	}

	TEST_F(MOS6502CMPTests, CpyZeroPage) {
		uint64_t UsedCycles, ExpectedCycles;
		Byte ins = CPU::INS_CPY_ZP;

		//Given:
		cpu.TestReset(CPU::RESET_VECTOR);	

		mem[0xFFFC] = ins;
		mem[0xFFFD] = 0x00;
		mem[0x0000] = 0x0F;
		cpu.Y = 0x2F;

		//When:
		std::tie(UsedCycles, ExpectedCycles) =
			cpu.executeOneInstruction();

		// Then:
		EXPECT_TRUE(cpu.Flags.C);
		EXPECT_FALSE(cpu.Flags.Z);
		EXPECT_FALSE(cpu.Flags.N);
		EXPECT_EQ(UsedCycles, ExpectedCycles); 
	}

	TEST_F(MOS6502CMPTests, CpyAbsolute) {
		uint64_t UsedCycles, ExpectedCycles;
		Byte ins = CPU::INS_CPY_ABS;

		//Given:
		cpu.TestReset(CPU::RESET_VECTOR);	

		mem[0xFFFC] = ins;
		mem[0xFFFD] = 0x00;
		mem[0xFFFE] = 0x20;
		mem[0x2000] = 0x0F;
		cpu.Y = 0x2F;

		//When:
		std::tie(UsedCycles, ExpectedCycles) =
			cpu.executeOneInstruction();

		// Then:
		EXPECT_TRUE(cpu.Flags.C);
		EXPECT_FALSE(cpu.Flags.Z);
		EXPECT_FALSE(cpu.Flags.N);
		EXPECT_EQ(UsedCycles, ExpectedCycles); 
	}
