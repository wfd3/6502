#include <gtest/gtest.h>
#include <6502.h>

class MOS6502DECTests : public testing::Test {
public:
	Memory<Address_t, Byte> mem{CPU::MAX_MEM};
	CPU cpu{mem};

	virtual void SetUp() {
		cpu.exitReset();
		mem.mapRAM(0, CPU::MAX_MEM);
	}

	virtual void TearDown()	{
	}
};

// DEC 
TEST_F(MOS6502DECTests, DecZeroPage) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_DEC_ZP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0x0020] = 0x02;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(mem[0x0020], 0x01);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502DECTests, DecZeroPageX) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_DEC_ZPX;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	cpu.X = 0x20;
	mem[0x0040] = 0x02;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(mem[0x0040], 0x01);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502DECTests, DecAbsolute) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_DEC_ABS;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x02;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(mem[0x2000], 0x01);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502DECTests, DecAbsoluteX) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_DEC_ABX;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.X = 0x02;
	mem[0x2002] = 0x02;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(mem[0x2002], 0x01);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502DECTests, DecZeroPageSetsZeroFlag) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_DEC_ZP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0x0020] = 0x01;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(mem[0x0020], 0x00);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}


TEST_F(MOS6502DECTests, DecZeroPageSetsNegativeFlag) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_DEC_ZP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0x0020] = 0x00;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(mem[0x0020], 0xff);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}


// DEX
TEST_F(MOS6502DECTests, DexImmediate) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_DEX_IMP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	cpu.X = 10;
	
	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.X, 9);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502DECTests, DexImmediateSetsZeroFlag) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_DEX_IMP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	cpu.X = 1;
	
	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.X, 0);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502DECTests, DexImmediateSetsNegativeFlag) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_DEX_IMP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	cpu.X = 0;
	
	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(cpu.X, 0xff);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// DEY
TEST_F(MOS6502DECTests, DeyImmediate) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_DEY_IMP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	cpu.Y = 10;
	
	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.Y, 9);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502DECTests, DeyImmediateSetsZeroFlag) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_DEY_IMP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	cpu.Y = 1;
	
	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.Y, 0);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502DECTests, DeyImmediateSetsNegativeFlag) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_DEY_IMP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	cpu.Y = 0;
	
	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(cpu.Y, 0xff);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
