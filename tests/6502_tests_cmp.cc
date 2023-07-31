#include <gtest/gtest.h>
#include "../6502.h"

extern	mos6502::MEMORY mem;
extern	mos6502::CPU cpu;

class MOS6502CMPTests : public testing::Test {
public:

	virtual void SetUp() {
	}
	
	virtual void TearDown()	{
	}
};

TEST_F(MOS6502CMPTests, CmpImmediate) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_CMP_IMM;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	cpu.A = 0x30;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CmpImmediateSetsCFlagFalse) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_CMP_IMM;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xFF;
	cpu.A = 0x0F;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CmpImmediateSetsZFlagTrue) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_CMP_IMM;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xFF;
	cpu.A = 0xFF;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CmpImmediateSetsNFlagTrue) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_CMP_IMM;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xFF;
	cpu.A = 0x0;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CmpZeroPage) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_CMP_ZP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0x0000] = 0x0F;
	cpu.A = 0xFF;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CmpZeroPageX) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_CMP_ZPX;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	cpu.X = 0x01;
	mem[0x0001] = 0x0F;
	cpu.A = 0xFF;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CmpAbsolute) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_CMP_ABS;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x0F;
	cpu.A = 0xFF;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CmpAbsoluteX) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_CMP_ABX;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.X = 0x01;
	mem[0x2001] = 0x0F;
	cpu.A = 0xFF;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CmpAbsoluteXCrossesPage) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_CMP_ABX;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	mem[0xFFFE] = 0x20;
	cpu.X = 0xFF;
	mem[0x210f] = 0x0F;
	cpu.A = 0xFF;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CmpAbsoluteY) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_CMP_ABY;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.Y = 0x01;
	mem[0x2001] = 0x0F;
	cpu.A = 0xFF;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CmpAbsoluteYCrossesPage) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_CMP_ABY;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	mem[0xFFFE] = 0x20;
	cpu.Y = 0xFF;
	mem[0x210f] = 0x0F;
	cpu.A = 0xFF;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CmpIndirectX) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_CMP_IDX;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x01;
	cpu.X = 0x01;
	mem[0x0002] = 0x01;
	mem[0x0000] = 0x20;
	mem[0x2001] = 0x0F;
	cpu.A = 0xFF;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CmpIndirectY) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_CMP_IDY;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x02;
	cpu.Y = 0x01;
	mem[0x0002] = 0x01;
	mem[0x0000] = 0x20;
	mem[0x2002] = 0x0F;
	cpu.A = 0xFF;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CmpIndirectYCrossesPage) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_CMP_IDY;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x02;
	cpu.Y = 0xFF;
	mem[0x0002] = 0x10;
	mem[0x0000] = 0x20;
	mem[0x210F] = 0x0F;
	cpu.A = 0xFF;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// CPX
TEST_F(MOS6502CMPTests, CpxImmediate) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_CPX_IMM;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	cpu.X = 0x30;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CpxImmediateSetsCFlagFalse) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_CPX_IMM;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xFF;
	cpu.X = 0x0F;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CpxImmediateSetsZFlagTrue) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_CPX_IMM;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xFF;
	cpu.X = 0xFF;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CpxImmediateSetsNFlagTrue) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_CPX_IMM;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xFF;
	cpu.X = 0x0;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CpxZeroPage) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_CPX_ZP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0x0000] = 0x0F;
	cpu.X = 0xFF;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CpxAbsolute) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_CPX_ABS;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x0F;
	cpu.X = 0xFF;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// CPY
TEST_F(MOS6502CMPTests, CpyImmediate) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_CPY_IMM;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	cpu.Y = 0x30;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CpyImmediateSetsCFlagFalse) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_CPY_IMM;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xFF;
	cpu.Y = 0x0F;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CpyImmediateSetsZFlagTrue) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_CPY_IMM;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xFF;
	cpu.Y = 0xFF;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CpyImmediateSetsNFlagTrue) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_CPY_IMM;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xFF;
	cpu.Y = 0x0;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CpyZeroPage) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_CPY_ZP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0x0000] = 0x0F;
	cpu.Y = 0xFF;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CpyAbsolute) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_CPY_ABS;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x0F;
	cpu.Y = 0xFF;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
