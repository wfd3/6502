#include <gtest/gtest.h>
#include "../6502.h"

extern	mos6502::MEMORY mem;
extern	mos6502::CPU cpu;

class MOS6502RORROLTests : public testing::Test {
public:

	virtual void SetUp() {
	}
	
	virtual void TearDown()	{
	}
};

TEST_F(MOS6502RORROLTests, RolAccumulator) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_ROL_ACC;
	mos6502::Byte data = 0b01010101;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	cpu.A = data;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(cpu.A, data << 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502RORROLTests, RolAccumulatorSetsCarryFlag) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_ROL_ACC;
	mos6502::Byte data = 0b10000001;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	cpu.A = data;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.A, mos6502::Byte(data << 1));
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502RORROLTests, RolZeroPage) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_ROL_ZP;
	mos6502::Byte data = 0b01010101;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0x0020] = data;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(mem[0x0020], data << 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502RORROLTests, RolZeroPageX) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_ROL_ZPX;
	mos6502::Byte data = 0b01010101;
	
	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	cpu.X = 0x10;
	mem[0x0020] = data;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(mem[0x0020], data << 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502RORROLTests, RolAbsoltute) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_ROL_ABS;
	mos6502::Byte data = 0b01010101;
	
	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = data;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(mem[0x2000], data << 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502RORROLTests, RolAbsoltuteX) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_ROL_ABX;
	mos6502::Byte data = 0b01010101;
	
	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.X = 0x05;
	mem[0x2005] = data;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(mem[0x2005], mos6502::Byte(data << 1));
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// ROR
TEST_F(MOS6502RORROLTests, RorAccumulator) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_ROR_ACC;
	mos6502::Byte data = 0b01010101;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	cpu.A = data;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.A, data >> 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502RORROLTests, RorAccumulatorSetsCarryFlag) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_ROR_ACC;
	mos6502::Byte data = 0b10000001;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	cpu.A = data;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.A, mos6502::Byte(data >> 1));
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502RORROLTests, RorAccumulatorClearsCarryAndSetsNegativeFlag) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_ROR_ACC;
	mos6502::Byte data = 0b10000000;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	cpu.A = data;
	cpu.Flags.C = 1;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(cpu.A, mos6502::Byte(data >> 1) | (1 << 7));
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502RORROLTests, RorZeroPage) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_ROR_ZP;
	mos6502::Byte data = 0b01010101;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0x0020] = data;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(mem[0x0020], data >> 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502RORROLTests, RorZeroPageX) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_ROR_ZPX;
	mos6502::Byte data = 0b01010101;
	
	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	cpu.X = 0x10;
	mem[0x0020] = data;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(mem[0x0020], data >> 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

//HERE
TEST_F(MOS6502RORROLTests, RorAbsoltute) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_ROR_ABS;
	mos6502::Byte data = 0b01010101;
	
	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = data;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(mem[0x2000], data >> 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502RORROLTests, RorAbsoltuteX) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_ROR_ABX;
	mos6502::Byte data = 0b01010101;
	
	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.X = 0x05;
	mem[0x2005] = data;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(mem[0x2005], mos6502::Byte(data >> 1));
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
