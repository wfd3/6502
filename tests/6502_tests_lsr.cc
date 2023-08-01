#include <gtest/gtest.h>
#include "../6502.h"

extern	mos6502::MEMORY mem;
extern	mos6502::CPU cpu;

class MOS6502LSRTests : public testing::Test {
public:

	virtual void SetUp() {
	}
	
	virtual void TearDown()	{
	}
};

TEST_F(MOS6502LSRTests, LsrAccumulator) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_LSR_ACC;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	cpu.A = 0b01010101;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.A, 0b00101010);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502LSRTests, LsrZeroPage) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_LSR_ZP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0x0020] = 0b01010101;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(mem[0x0020], 0b00101010);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502LSRTests, LsrZeroPageX) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_LSR_ZPX;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	cpu.X = 0x10;
	mem[0x0020] = 0b01010101;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(mem[0x0020], 0b00101010);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502LSRTests, LsrAbsoltute) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_LSR_ABS;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0b01010101;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(mem[0x2000], 0b00101010);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502LSRTests, LsrAbsoltuteX) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_LSR_ABX;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.X = 0x05;
	mem[0x2005] = 0b01010101;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(mem[0x2005], 0b00101010);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

