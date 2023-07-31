#include <gtest/gtest.h>
#include "../6502.h"

extern	mos6502::MEMORY mem;
extern	mos6502::CPU cpu;

class MOS6502DECTests : public testing::Test {
public:

	virtual void SetUp() {
	}
	
	virtual void TearDown()	{
	}
};

// DEC 
TEST_F(MOS6502DECTests, DecZeroPage) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_DEC_ZP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0x0020] = 0x02;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(mem[0x0020], 0x01);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502DECTests, DecZeroPageX) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_DEC_ZPX;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	cpu.X = 0x20;
	mem[0x0040] = 0x02;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(mem[0x0040], 0x01);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502DECTests, DecAbsolute) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_DEC_ABS;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x02;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(mem[0x2000], 0x01);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502DECTests, DecAbsoluteX) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_DEC_ABX;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.X = 0x02;
	mem[0x2002] = 0x02;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(mem[0x2002], 0x01);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502DECTests, DecZeroPageSetsZeroFlag) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_DEC_ZP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0x0020] = 0x01;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(mem[0x0020], 0x00);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}


TEST_F(MOS6502DECTests, DecZeroPageSetsNegativeFlag) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_DEC_ZP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0x0020] = 0x00;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(mem[0x0020], 0xff);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}


// DEX
TEST_F(MOS6502DECTests, DexImmediate) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_DEX_IMP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	cpu.X = 10;
	
	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.X, 9);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502DECTests, DexImmediateSetsZeroFlag) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_DEX_IMP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	cpu.X = 1;
	
	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.X, 0);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502DECTests, DexImmediateSetsNegativeFlag) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_DEX_IMP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	cpu.X = 0;
	
	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(cpu.X, 0xff);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// DEY
TEST_F(MOS6502DECTests, DeyImmediate) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_DEY_IMP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	cpu.Y = 10;
	
	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.Y, 9);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502DECTests, DeyImmediateSetsZeroFlag) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_DEY_IMP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	cpu.Y = 1;
	
	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.Y, 0);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502DECTests, DeyImmediateSetsNegativeFlag) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_DEY_IMP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	cpu.Y = 0;
	
	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(cpu.Y, 0xff);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
