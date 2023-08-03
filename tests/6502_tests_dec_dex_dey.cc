#include <gtest/gtest.h>
#include "../6502.h"

class MOS6502DECTests : public testing::Test {
public:
	Memory mem{CPU::MAX_MEM};
	CPU cpu{&mem};

	virtual void SetUp() {
		cpu.Reset(CPU::INITIAL_PC);
		mem.Init();
	}

	virtual void TearDown()	{
	}
};

// DEC 
TEST_F(MOS6502DECTests, DecZeroPage) {
	CPU::Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_DEC_ZP;

	//Given:
	cpu.Reset(CPU::INITIAL_PC);
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
	CPU::Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_DEC_ZPX;

	//Given:
	cpu.Reset(CPU::INITIAL_PC);
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
	CPU::Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_DEC_ABS;

	//Given:
	cpu.Reset(CPU::INITIAL_PC);
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
	CPU::Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_DEC_ABX;

	//Given:
	cpu.Reset(CPU::INITIAL_PC);
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
	CPU::Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_DEC_ZP;

	//Given:
	cpu.Reset(CPU::INITIAL_PC);
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
	CPU::Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_DEC_ZP;

	//Given:
	cpu.Reset(CPU::INITIAL_PC);
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
	CPU::Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_DEX_IMP;

	//Given:
	cpu.Reset(CPU::INITIAL_PC);
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
	CPU::Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_DEX_IMP;

	//Given:
	cpu.Reset(CPU::INITIAL_PC);
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
	CPU::Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_DEX_IMP;

	//Given:
	cpu.Reset(CPU::INITIAL_PC);
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
	CPU::Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_DEY_IMP;

	//Given:
	cpu.Reset(CPU::INITIAL_PC);
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
	CPU::Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_DEY_IMP;

	//Given:
	cpu.Reset(CPU::INITIAL_PC);
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
	CPU::Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_DEY_IMP;

	//Given:
	cpu.Reset(CPU::INITIAL_PC);
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
