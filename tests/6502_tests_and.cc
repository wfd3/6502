#include <gtest/gtest.h>
#include "../6502.h"

extern	mos6502::MEMORY mem;
extern	mos6502::CPU cpu;

class MOS6502ANDTests : public testing::Test {
public:

	virtual void SetUp() {
	}
	
	virtual void TearDown()	{
	}
};

TEST_F(MOS6502ANDTests, AndImmediate) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_AND_IMM;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x0F;
	cpu.A = 0xFF;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, 0x0F);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ANDTests, AndImmediateSetsZeroFlag) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_AND_IMM;

	mos6502::Cycles_t c = cpu.Cycles;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	cpu.A = 0xFF;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, 0x00);
	EXPECT_GT(cpu.Cycles, c);
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ANDTests, AndZeroPage) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_AND_ZP;
	
	// Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	mem[0x0010] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ANDTests, AndZeroPageX) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_AND_ZPX;
	
	// Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x01;
	cpu.X = 0x80;
	mem[0x81] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ANDTests, AndAbsolute) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_AND_ABS;
	
	// Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ANDTests, AndAbsoluteX) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_AND_ABX;
	
	// Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.X = 0x10;
	mem[0x2010] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ANDTests, AndAbsoluteXCrossesPage) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_AND_ABX;
	
	// Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xFF;
	mem[0xFFFE] = 0x20;
	cpu.X = 0x10;
	mem[0x20FF + 0x10] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles);
}

TEST_F(MOS6502ANDTests, AndAbsoluteY) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_AND_ABY;
	
	// Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.Y = 0x10;
	mem[0x2010] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ANDTests, AndAbsoluteYCrossesPage) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_AND_ABY;
	
	// Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xFF;
	mem[0xFFFE] = 0x20;
	cpu.Y = 0x10;
	mem[0x20FF + 0x10] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles);
}

TEST_F(MOS6502ANDTests, AndIndirectX) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_AND_IDX;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	cpu.X = 0x10;
	mem[0x0020] = 0x00;
	mem[0x0021] = 0x20;
	mem[0x2000] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles);
}

TEST_F(MOS6502ANDTests, AndIndirectY) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_AND_IDY;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	mem[0x0010] = 0x00;
	mem[0x0011] = 0x20;
	cpu.Y = 0x10;
	mem[0x2000 + 0x10] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();	

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles);
}
