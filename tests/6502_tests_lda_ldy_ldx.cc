#include <gtest/gtest.h>
#include "../6502.h"

extern mos6502::MEMORY mem;
extern mos6502::CPU cpu;

class MOS6502LDTests : public testing::Test {
public:

	virtual void SetUp() {
	}
	
	virtual void TearDown()	{
	}
};

void TestLD(mos6502::Byte ins, mos6502::Byte &reg) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);

	mem[0xFFFC] = ins;
	reg = 0xFF;
	
	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(reg, 0x0F);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// LDA
TEST_F(MOS6502LDTests, LDAImmediate) {
	mem[0xFFFD] = 0x0F;
	TestLD(mos6502::CPU::INS_LDA_IMM, cpu.A);
}

TEST_F(MOS6502LDTests, LDAZeroPage) {
	mem[0xFFFD] = 0x0F;
	mem[0x000F] = 0x0F;
	TestLD(mos6502::CPU::INS_LDA_ZP, cpu.A);
}
TEST_F(MOS6502LDTests, LDAZeroPageX) {
	mem[0xFFFD] = 0x0F;
	mem[0x000F] = 0x0F;
	cpu.X = 0;
	TestLD(mos6502::CPU::INS_LDA_ZPX, cpu.A);
}
TEST_F(MOS6502LDTests, LDAAbsolute) {
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x0F;
	TestLD(mos6502::CPU::INS_LDA_ABS, cpu.A);
}
TEST_F(MOS6502LDTests, LDAAbsoluteX) {
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.X = 0;
	mem[0x2000] = 0x0F;
	TestLD(mos6502::CPU::INS_LDA_ABX, cpu.A);
}

// LDX
TEST_F(MOS6502LDTests, LDXImmediate) {
	mem[0xFFFD] = 0x0F;
	TestLD(mos6502::CPU::INS_LDX_IMM, cpu.X);
}

TEST_F(MOS6502LDTests, LDXZeroPage) {
	mem[0xFFFD] = 0x0F;
	mem[0x000F] = 0x0F;
	TestLD(mos6502::CPU::INS_LDX_ZP, cpu.X);
}
TEST_F(MOS6502LDTests, LDXZeroPageX) {
	mem[0xFFFD] = 0x0F;
	mem[0x000F] = 0x0F;
	cpu.Y = 0;
	TestLD(mos6502::CPU::INS_LDX_ZPY, cpu.X);
}
TEST_F(MOS6502LDTests, LDXAbsolute) {
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x0F;
	TestLD(mos6502::CPU::INS_LDX_ABS, cpu.X);
}
TEST_F(MOS6502LDTests, LDXAbsoluteX) {
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.Y = 0;
	mem[0x2000] = 0x0F;
	TestLD(mos6502::CPU::INS_LDY_ABX, cpu.Y);
}

// LDY
TEST_F(MOS6502LDTests, LDYImmediate) {
	mem[0xFFFD] = 0x0F;
	TestLD(mos6502::CPU::INS_LDY_IMM, cpu.Y);
}

TEST_F(MOS6502LDTests, LDYZeroPage) {
	mem[0xFFFD] = 0x0F;
	mem[0x000F] = 0x0F;
	TestLD(mos6502::CPU::INS_LDY_ZP, cpu.Y);
}
TEST_F(MOS6502LDTests, LDYZeroPageX) {
	mem[0xFFFD] = 0x0F;
	mem[0x000F] = 0x0F;
	cpu.X = 0;
	TestLD(mos6502::CPU::INS_LDY_ZPX, cpu.Y);
}
TEST_F(MOS6502LDTests, LDYAbsolute) {
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x0F;
	TestLD(mos6502::CPU::INS_LDY_ABS, cpu.Y);
}
TEST_F(MOS6502LDTests, LDYAbsoluteX) {
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.X = 0;
	mem[0x2000] = 0x0F;
	TestLD(mos6502::CPU::INS_LDY_ABX, cpu.Y);
}



