#include <gtest/gtest.h>
#include <6502.h>

class MOS6502TXTYTests : public testing::Test {
public:

	Memory<Address_t, Byte> mem{CPU::MAX_MEM};
	CPU cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, CPU::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}
};

// TAX

TEST_F(MOS6502TXTYTests, TAX) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_TAX_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.A = 0x52;
	cpu.X = 0xAB;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.X, cpu.A);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502TXTYTests, TAXSetsZeroFlag) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_TAX_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.A = 0x0;
	cpu.X = 0xAB;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.X, cpu.A);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502TXTYTests, TAXSetsNegativeFlag) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_TAX_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.A = 0xFF;
	cpu.X = 0xAB;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(cpu.X, cpu.A);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}


// TXA

TEST_F(MOS6502TXTYTests, TXA) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_TXA_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.A = 0x52;
	cpu.X = 0x0B;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.X, cpu.A);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502TXTYTests, TXASetsZeroFlag) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_TXA_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.X = 0x0;
	cpu.A = 0xAB;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.X, cpu.A);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502TXTYTests, TXASetsNegativeFlag) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_TXA_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.X = 0xFF;
	cpu.A = 0xAB;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(cpu.X, cpu.A);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// TAY

TEST_F(MOS6502TXTYTests, TAY) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_TAY_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.A = 0x52;
	cpu.Y = 0x0B;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.Y, cpu.A);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502TXTYTests, TAYSetsZeroFlag) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_TAY_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.A = 0x0;
	cpu.Y = 0xAB;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.Y, cpu.A);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502TXTYTests, TAYSetsNegativeFlag) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_TAY_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.A = 0xFF;
	cpu.Y = 0xAB;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(cpu.Y, cpu.A);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// TYA

TEST_F(MOS6502TXTYTests, TYA) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_TYA_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.A = 0x52;
	cpu.Y = 0x0B;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.Y, cpu.A);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502TXTYTests, TYASetsZeroFlag) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_TYA_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.Y = 0x0;
	cpu.A = 0xAB;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.Y, cpu.A);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502TXTYTests, TYASetsNegativeFlag) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_TYA_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.Y = 0xFF;
	cpu.A = 0xAB;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(cpu.Y, cpu.A);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// TSX

TEST_F(MOS6502TXTYTests, TSX) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_TSX_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR, 0x12);
	
	mem[0xFFFC] = ins;
	cpu.X = 0x52;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.X, cpu.SP);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502TXTYTests, TSXSetsZeroFlag) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_TSX_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR, 0);
	
	mem[0xFFFC] = ins;
	cpu.X = 0xAB;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.X, cpu.SP);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502TXTYTests, TSXSetsNegativeFlag) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_TSX_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR, 0xff);
	
	mem[0xFFFC] = ins;
	cpu.X = 0xAB;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(cpu.X, cpu.SP);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// TXS

TEST_F(MOS6502TXTYTests, TXS) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_TXS_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR, 0x12);
	
	mem[0xFFFC] = ins;
	cpu.X = 0x52;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.X, cpu.SP);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

