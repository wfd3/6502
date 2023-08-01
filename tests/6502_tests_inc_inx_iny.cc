#include <gtest/gtest.h>
#include "../6502.h"

extern	mos6502::MEMORY mem;
extern	mos6502::CPU cpu;

class MOS6502INCTests : public testing::Test {
public:

	virtual void SetUp() {
	}
	
	virtual void TearDown()	{
	}
};

// DEC 
TEST_F(MOS6502INCTests, IncZeroPage) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_INC_ZP;

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
	EXPECT_EQ(mem[0x0020], 0x03);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502INCTests, IncZeroPageX) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_INC_ZPX;

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
	EXPECT_EQ(mem[0x0040], 0x03);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502INCTests, IncAbsolute) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_INC_ABS;

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
	EXPECT_EQ(mem[0x2000], 0x03);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502INCTests, IncAbsoluteX) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_INC_ABX;

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
	EXPECT_EQ(mem[0x2002], 0x03);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502INCTests, IncZeroPageSetsZeroFlag) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_INC_ZP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0x0020] = 0xff;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(mem[0x0020], 0x00);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}


TEST_F(MOS6502INCTests, IncZeroPageSetsNegativeFlag) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_INC_ZP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0x0020] = 0x7f;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(mem[0x0020], 0x80);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}


// INX
TEST_F(MOS6502INCTests, InxImplied) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_INX_IMP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	cpu.X = 0x09;
	
	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.X, 0x0a);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502INCTests, InxImpliedSetsZeroFlag) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_INX_IMP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	cpu.X = 0xff;
	
	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.X, 0);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502INCTests, InxImpliedSetsNegativeFlag) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_INX_IMP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	cpu.X = 127;
	
	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(cpu.X, 128);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// INY
TEST_F(MOS6502INCTests, InyImplied) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_INY_IMP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	cpu.Y = 0x09;
	
	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.Y, 0x0a);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502INCTests, InyImpliedSetsZeroFlag) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_INY_IMP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	cpu.Y = 0xff;
	
	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.Y, 0);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502INCTests, InyImpliedSetsNegativeFlag) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_INY_IMP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	cpu.Y = 127;
	
	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(cpu.Y, 128);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
