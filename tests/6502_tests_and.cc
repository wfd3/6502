#include <gtest/gtest.h>
#include <6502.h>


class MOS6502ANDTests : public testing::Test {
public:
	Memory<Address_t, Byte> mem{CPU::MAX_MEM};
	CPU cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, CPU::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}
};

TEST_F(MOS6502ANDTests, AndImmediate) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_AND_IMM;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x0F;
	cpu.A = 0xFF;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.A, 0x0F);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ANDTests, AndImmediateSetsZeroFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_AND_IMM;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	cpu.A = 0xFF;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.A, 0x00);
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ANDTests, AndZeroPage) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_AND_ZP;
	
	// Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	mem[0x0010] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ANDTests, AndZeroPageX) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_AND_ZPX;
	
	// Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x01;
	cpu.X = 0x80;
	mem[0x81] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ANDTests, AndAbsolute) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_AND_ABS;
	
	// Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ANDTests, AndAbsoluteX) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_AND_ABX;
	
	// Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.X = 0x10;
	mem[0x2010] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ANDTests, AndAbsoluteXCrossesPage) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_AND_ABX;
	
	// Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xFF;
	mem[0xFFFE] = 0x20;
	cpu.X = 0x10;
	mem[0x20FF + 0x10] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles);
}

TEST_F(MOS6502ANDTests, AndAbsoluteY) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_AND_ABY;
	
	// Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.Y = 0x10;
	mem[0x2010] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ANDTests, AndAbsoluteYCrossesPage) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_AND_ABY;
	
	// Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xFF;
	mem[0xFFFE] = 0x20;
	cpu.Y = 0x10;
	mem[0x20FF + 0x10] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles);
}

TEST_F(MOS6502ANDTests, AndIndirectX) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_AND_IDX;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	cpu.X = 0x10;
	mem[0x0020] = 0x00;
	mem[0x0021] = 0x20;
	mem[0x2000] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles);
}

TEST_F(MOS6502ANDTests, AndIndirectY) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_AND_IDY;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	mem[0x0010] = 0x00;
	mem[0x0011] = 0x20;
	cpu.Y = 0x10;
	mem[0x2000 + 0x10] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);	

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles);

}
