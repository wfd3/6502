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

// Test STA, STX and STY
void TestST(mos6502::Byte ins, mos6502::Word addr, mos6502::Byte &reg) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;

	//Given:
	mem[0xFFFC] = ins;

	// When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	//Then:
	EXPECT_EQ(mem[addr], reg);
}
	 
// LDA
TEST_F(MOS6502LDTests, LDAImmediate) {
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFD] = 0x0F;
	TestLD(mos6502::CPU::INS_LDA_IMM, cpu.A);
}

TEST_F(MOS6502LDTests, LDAZeroPage) {
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFD] = 0x0F;
	mem[0x000F] = 0x0F;
	TestLD(mos6502::CPU::INS_LDA_ZP, cpu.A);
}
TEST_F(MOS6502LDTests, LDAZeroPageX) {
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFD] = 0x0F;
	mem[0x000F] = 0x0F;
	cpu.X = 0;
	TestLD(mos6502::CPU::INS_LDA_ZPX, cpu.A);
}
TEST_F(MOS6502LDTests, LDAAbsolute) {
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x0F;
	TestLD(mos6502::CPU::INS_LDA_ABS, cpu.A);
}
TEST_F(MOS6502LDTests, LDAAbsoluteX) {
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.X = 0;
	mem[0x2000] = 0x0F;
	TestLD(mos6502::CPU::INS_LDA_ABX, cpu.A);
}

// LDX
TEST_F(MOS6502LDTests, LDXImmediate) {
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFD] = 0x0F;
	TestLD(mos6502::CPU::INS_LDX_IMM, cpu.X);
}
TEST_F(MOS6502LDTests, LDXZeroPage) {
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFD] = 0x0F;
	mem[0x000F] = 0x0F;
	TestLD(mos6502::CPU::INS_LDX_ZP, cpu.X);
}
TEST_F(MOS6502LDTests, LDXZeroPageY) {
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFD] = 0x0F;
	mem[0x000F] = 0x0F;
	cpu.Y = 0;
	TestLD(mos6502::CPU::INS_LDX_ZPY, cpu.X);
}
TEST_F(MOS6502LDTests, LDXAbsolute) {
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x0F;
	TestLD(mos6502::CPU::INS_LDX_ABS, cpu.X);
}
TEST_F(MOS6502LDTests, LDXAbsoluteX) {
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.Y = 0;
	mem[0x2000] = 0x0F;
	TestLD(mos6502::CPU::INS_LDY_ABX, cpu.Y);
}

// LDY
TEST_F(MOS6502LDTests, LDYImmediate) {
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFD] = 0x0F;
	TestLD(mos6502::CPU::INS_LDY_IMM, cpu.Y);
}

TEST_F(MOS6502LDTests, LDYZeroPage) {
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFD] = 0x0F;
	mem[0x000F] = 0x0F;
	TestLD(mos6502::CPU::INS_LDY_ZP, cpu.Y);
}
TEST_F(MOS6502LDTests, LDYZeroPageX) {
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFD] = 0x0F;
	mem[0x000F] = 0x0F;
	cpu.X = 0;
	TestLD(mos6502::CPU::INS_LDY_ZPX, cpu.Y);
}
TEST_F(MOS6502LDTests, LDYAbsolute) {
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x0F;
	TestLD(mos6502::CPU::INS_LDY_ABS, cpu.Y);
}
TEST_F(MOS6502LDTests, LDYAbsoluteX) {
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.X = 0;
	mem[0x2000] = 0x0F;
	TestLD(mos6502::CPU::INS_LDY_ABX, cpu.Y);
}

// STA
TEST_F(MOS6502LDTests, STAAbsolute) {
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.A = 0x52;
	TestST(mos6502::CPU::INS_STA_ABS, 0x2000, cpu.A);
}

TEST_F(MOS6502LDTests, STAZeroPage) {
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFD] = 0x20;
	cpu.A = 0x52;
	TestST(mos6502::CPU::INS_STA_ZP, 0x0020, cpu.A);
}

TEST_F(MOS6502LDTests, STAZeroPageX) {
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFD] = 0x20;
	cpu.X = 0x02;
	cpu.A = 0x52;
	TestST(mos6502::CPU::INS_STA_ZPX, 0x0022, cpu.A);
}

TEST_F(MOS6502LDTests, STAAbsoluteX) {
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.X = 0x02;
	cpu.A = 0x52;
	TestST(mos6502::CPU::INS_STA_ABX, 0x2002, cpu.A);
}

TEST_F(MOS6502LDTests, STAAbsoluteY) {
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.Y = 0x02;
	cpu.A = 0x52;
	TestST(mos6502::CPU::INS_STA_ABY, 0x2002, cpu.A);
}

TEST_F(MOS6502LDTests, STAIndirectX) {
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFD] = 0x10;
	cpu.X = 0x10;
	mem[0x0020] = 0x00;
	mem[0x0021] = 0x20;

	cpu.A = 0x52;
	TestST(mos6502::CPU::INS_STA_IDX, 0x2000, cpu.A);
}

TEST_F(MOS6502LDTests, STAIndirectY) {
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFD] = 0x00;
	mem[0x0000] = 0x00;
	mem[0x0001] = 0x20;
	cpu.Y = 0x02;
	cpu.A = 0x52;
	TestST(mos6502::CPU::INS_STA_IDY, 0x2002, cpu.A);
}

// STX
TEST_F(MOS6502LDTests, STXZeroPage) {
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFD] = 0x20;
	cpu.X = 0x52;
	TestST(mos6502::CPU::INS_STX_ZP, 0x0020, cpu.X);
}

TEST_F(MOS6502LDTests, STXZeroPageY) {
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFD] = 0x20;
	cpu.Y = 0x02;
	cpu.X = 0x52;
	TestST(mos6502::CPU::INS_STX_ZPY, 0x0022, cpu.X);
}

TEST_F(MOS6502LDTests, STXAbsolute) {
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.X = 0x52;
	TestST(mos6502::CPU::INS_STX_ABS, 0x2000, cpu.X);
}

// STY
TEST_F(MOS6502LDTests, STYZeroPage) {
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFD] = 0x20;
	cpu.Y = 0x52;
	TestST(mos6502::CPU::INS_STY_ZP, 0x0020, cpu.Y);
}

TEST_F(MOS6502LDTests, STYZeroPageX) {
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFD] = 0x20;
	cpu.X = 0x02;
	cpu.Y = 0x52;
	TestST(mos6502::CPU::INS_STY_ZPX, 0x0022, cpu.Y);
}

TEST_F(MOS6502LDTests, STYAbsolute) {
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.Y = 0x52;
	TestST(mos6502::CPU::INS_STY_ABS, 0x2000, cpu.Y);
}
