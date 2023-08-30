#include <gtest/gtest.h>
#include <6502.h>


class MOS6502ANDTests : public testing::Test {
public:
	Memory<Address_t, Byte> mem{CPU::MAX_MEM};
	CPU cpu{&mem};

	virtual void SetUp() {
		cpu.exitReset();
		mem.mapRAM(0, CPU::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}
};

TEST_F(MOS6502ANDTests, AndImmediate) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_AND_IMM;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x0F;
	cpu.A = 0xFF;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, 0x0F);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ANDTests, AndImmediateSetsZeroFlag) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_AND_IMM;

	Byte c = cpu.Cycles.get(); 

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	cpu.A = 0xFF;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, 0x00);
	EXPECT_GT(cpu.Cycles, c);
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ANDTests, AndZeroPage) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_AND_ZP;
	
	// Given:
	cpu.Reset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	mem[0x0010] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ANDTests, AndZeroPageX) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_AND_ZPX;
	
	// Given:
	cpu.Reset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x01;
	cpu.X = 0x80;
	mem[0x81] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ANDTests, AndAbsolute) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_AND_ABS;
	
	// Given:
	cpu.Reset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ANDTests, AndAbsoluteX) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_AND_ABX;
	
	// Given:
	cpu.Reset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.X = 0x10;
	mem[0x2010] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ANDTests, AndAbsoluteXCrossesPage) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_AND_ABX;
	
	// Given:
	cpu.Reset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xFF;
	mem[0xFFFE] = 0x20;
	cpu.X = 0x10;
	mem[0x20FF + 0x10] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles);
}

TEST_F(MOS6502ANDTests, AndAbsoluteY) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_AND_ABY;
	
	// Given:
	cpu.Reset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.Y = 0x10;
	mem[0x2010] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ANDTests, AndAbsoluteYCrossesPage) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_AND_ABY;
	
	// Given:
	cpu.Reset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xFF;
	mem[0xFFFE] = 0x20;
	cpu.Y = 0x10;
	mem[0x20FF + 0x10] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles);
}

TEST_F(MOS6502ANDTests, AndIndirectX) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_AND_IDX;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	cpu.X = 0x10;
	mem[0x0020] = 0x00;
	mem[0x0021] = 0x20;
	mem[0x2000] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles);
}

TEST_F(MOS6502ANDTests, AndIndirectY) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_AND_IDY;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	mem[0x0010] = 0x00;
	mem[0x0011] = 0x20;
	cpu.Y = 0x10;
	mem[0x2000 + 0x10] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();	

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles);

}
