#include <gtest/gtest.h>
#include <6502.h>

class MOS6502LDTests : public testing::Test {
public:

	Memory<Address_t, Byte> mem{CPU::MAX_MEM};
	CPU cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, CPU::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}
	
	void TestLD(Byte, Byte &);
	void TestST(Byte, Word, Byte &);
};

void MOS6502LDTests::TestLD(Byte ins, Byte &reg) {
	Cycles_t UsedCycles, ExpectedCycles;

	//Given:

	mem[0xFFFC] = ins;
	reg = 0xFF;
	
	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(reg, 0x0F);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// Test STA, STX and STY
void MOS6502LDTests::TestST(Byte ins, Word addr, Byte &reg) {
	Cycles_t UsedCycles, ExpectedCycles;

	//Given:
	mem[0xFFFC] = ins;

	// When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	//Then:
	EXPECT_EQ(mem[addr], reg);
}
	 
// LDA
TEST_F(MOS6502LDTests, LDAImmediate) {
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFD] = 0x0F;
	TestLD(Opcodes::INS_LDA_IMM, cpu.A);
}

TEST_F(MOS6502LDTests, LDAZeroPage) {
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFD] = 0x0F;
	mem[0x000F] = 0x0F;
	TestLD(Opcodes::INS_LDA_ZP, cpu.A);
}
TEST_F(MOS6502LDTests, LDAZeroPageX) {
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFD] = 0x0F;
	mem[0x000F] = 0x0F;
	cpu.X = 0;
	TestLD(Opcodes::INS_LDA_ZPX, cpu.A);
}
TEST_F(MOS6502LDTests, LDAAbsolute) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x0F;
	TestLD(Opcodes::INS_LDA_ABS, cpu.A);
}
TEST_F(MOS6502LDTests, LDAAbsoluteX) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.X = 0;
	mem[0x2000] = 0x0F;
	TestLD(Opcodes::INS_LDA_ABX, cpu.A);
}

// LDX
TEST_F(MOS6502LDTests, LDXImmediate) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	
	mem[0xFFFD] = 0x0F;
	TestLD(Opcodes::INS_LDX_IMM, cpu.X);
}
TEST_F(MOS6502LDTests, LDXZeroPage) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	
	mem[0xFFFD] = 0x0F;
	mem[0x000F] = 0x0F;
	TestLD(Opcodes::INS_LDX_ZP, cpu.X);
}
TEST_F(MOS6502LDTests, LDXZeroPageY) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	
	mem[0xFFFD] = 0x0F;
	mem[0x000F] = 0x0F;
	cpu.Y = 0;
	TestLD(Opcodes::INS_LDX_ZPY, cpu.X);
}
TEST_F(MOS6502LDTests, LDXAbsolute) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x0F;
	TestLD(Opcodes::INS_LDX_ABS, cpu.X);
}
TEST_F(MOS6502LDTests, LDXAbsoluteX) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.Y = 0;
	mem[0x2000] = 0x0F;
	TestLD(Opcodes::INS_LDY_ABX, cpu.Y);
}

// LDY
TEST_F(MOS6502LDTests, LDYImmediate) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	
	mem[0xFFFD] = 0x0F;
	TestLD(Opcodes::INS_LDY_IMM, cpu.Y);
}

TEST_F(MOS6502LDTests, LDYZeroPage) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	
	mem[0xFFFD] = 0x0F;
	mem[0x000F] = 0x0F;
	TestLD(Opcodes::INS_LDY_ZP, cpu.Y);
}
TEST_F(MOS6502LDTests, LDYZeroPageX) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	
	mem[0xFFFD] = 0x0F;
	mem[0x000F] = 0x0F;
	cpu.X = 0;
	TestLD(Opcodes::INS_LDY_ZPX, cpu.Y);
}
TEST_F(MOS6502LDTests, LDYAbsolute) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x0F;
	TestLD(Opcodes::INS_LDY_ABS, cpu.Y);
}
TEST_F(MOS6502LDTests, LDYAbsoluteX) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.X = 0;
	mem[0x2000] = 0x0F;
	TestLD(Opcodes::INS_LDY_ABX, cpu.Y);
}

// STA
TEST_F(MOS6502LDTests, STAAbsolute) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.A = 0x52;
	TestST(Opcodes::INS_STA_ABS, 0x2000, cpu.A);
}

TEST_F(MOS6502LDTests, STAZeroPage) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	
	mem[0xFFFD] = 0x20;
	cpu.A = 0x52;
	TestST(Opcodes::INS_STA_ZP, 0x0020, cpu.A);
}

TEST_F(MOS6502LDTests, STAZeroPageX) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	
	mem[0xFFFD] = 0x20;
	cpu.X = 0x02;
	cpu.A = 0x52;
	TestST(Opcodes::INS_STA_ZPX, 0x0022, cpu.A);
}

TEST_F(MOS6502LDTests, STAAbsoluteX) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.X = 0x02;
	cpu.A = 0x52;
	TestST(Opcodes::INS_STA_ABX, 0x2002, cpu.A);
}

TEST_F(MOS6502LDTests, STAAbsoluteY) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.Y = 0x02;
	cpu.A = 0x52;
	TestST(Opcodes::INS_STA_ABY, 0x2002, cpu.A);
}

TEST_F(MOS6502LDTests, STAIndirectX) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	
	mem[0xFFFD] = 0x10;
	cpu.X = 0x10;
	mem[0x0020] = 0x00;
	mem[0x0021] = 0x20;

	cpu.A = 0x52;
	TestST(Opcodes::INS_STA_IDX, 0x2000, cpu.A);
}

TEST_F(MOS6502LDTests, STAIndirectY) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	
	mem[0xFFFD] = 0x00;
	mem[0x0000] = 0x00;
	mem[0x0001] = 0x20;
	cpu.Y = 0x02;
	cpu.A = 0x52;
	TestST(Opcodes::INS_STA_IDY, 0x2002, cpu.A);
}

// STX
TEST_F(MOS6502LDTests, STXZeroPage) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	
	mem[0xFFFD] = 0x20;
	cpu.X = 0x52;
	TestST(Opcodes::INS_STX_ZP, 0x0020, cpu.X);
}

TEST_F(MOS6502LDTests, STXZeroPageY) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	
	mem[0xFFFD] = 0x20;
	cpu.Y = 0x02;
	cpu.X = 0x52;
	TestST(Opcodes::INS_STX_ZPY, 0x0022, cpu.X);
}

TEST_F(MOS6502LDTests, STXAbsolute) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.X = 0x52;
	TestST(Opcodes::INS_STX_ABS, 0x2000, cpu.X);
}

// STY
TEST_F(MOS6502LDTests, STYZeroPage) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	
	mem[0xFFFD] = 0x20;
	cpu.Y = 0x52;
	TestST(Opcodes::INS_STY_ZP, 0x0020, cpu.Y);
}

TEST_F(MOS6502LDTests, STYZeroPageX) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	
	mem[0xFFFD] = 0x20;
	cpu.X = 0x02;
	cpu.Y = 0x52;
	TestST(Opcodes::INS_STY_ZPX, 0x0022, cpu.Y);
}

TEST_F(MOS6502LDTests, STYAbsolute) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.Y = 0x52;
	TestST(Opcodes::INS_STY_ABS, 0x2000, cpu.Y);
}
