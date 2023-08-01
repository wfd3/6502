#include <gtest/gtest.h>
#include "../6502.h"

extern	mos6502::MEMORY mem;
extern	mos6502::CPU cpu;

class MOS6502TXTYTests : public testing::Test {
public:

	virtual void SetUp() {
	}
	
	virtual void TearDown()	{
	}
};

// TAX

TEST_F(MOS6502TXTYTests, TAX) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_TAX_IMP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	mem[0xFFFC] = ins;
	cpu.A = 0x52;
	cpu.X = 0xAB;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.X, cpu.A);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502TXTYTests, TAXSetsZeroFlag) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_TAX_IMP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	mem[0xFFFC] = ins;
	cpu.A = 0x0;
	cpu.X = 0xAB;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.X, cpu.A);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502TXTYTests, TAXSetsNegativeFlag) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_TAX_IMP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	mem[0xFFFC] = ins;
	cpu.A = 0xFF;
	cpu.X = 0xAB;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(cpu.X, cpu.A);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}


// TXA

TEST_F(MOS6502TXTYTests, TXA) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_TXA_IMP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	mem[0xFFFC] = ins;
	cpu.A = 0x52;
	cpu.X = 0x0B;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.X, cpu.A);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502TXTYTests, TXASetsZeroFlag) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_TXA_IMP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	mem[0xFFFC] = ins;
	cpu.X = 0x0;
	cpu.A = 0xAB;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.X, cpu.A);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502TXTYTests, TXASetsNegativeFlag) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_TXA_IMP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	mem[0xFFFC] = ins;
	cpu.X = 0xFF;
	cpu.A = 0xAB;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(cpu.X, cpu.A);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// TAY

TEST_F(MOS6502TXTYTests, TAY) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_TAY_IMP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	mem[0xFFFC] = ins;
	cpu.A = 0x52;
	cpu.Y = 0x0B;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.Y, cpu.A);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502TXTYTests, TAYSetsZeroFlag) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_TAY_IMP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	mem[0xFFFC] = ins;
	cpu.A = 0x0;
	cpu.Y = 0xAB;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.Y, cpu.A);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502TXTYTests, TAYSetsNegativeFlag) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_TAY_IMP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	mem[0xFFFC] = ins;
	cpu.A = 0xFF;
	cpu.Y = 0xAB;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(cpu.Y, cpu.A);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// TYA

TEST_F(MOS6502TXTYTests, TYA) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_TYA_IMP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	mem[0xFFFC] = ins;
	cpu.A = 0x52;
	cpu.Y = 0x0B;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.Y, cpu.A);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502TXTYTests, TYASetsZeroFlag) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_TYA_IMP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	mem[0xFFFC] = ins;
	cpu.Y = 0x0;
	cpu.A = 0xAB;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.Y, cpu.A);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502TXTYTests, TYASetsNegativeFlag) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_TYA_IMP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	mem[0xFFFC] = ins;
	cpu.Y = 0xFF;
	cpu.A = 0xAB;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(cpu.Y, cpu.A);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// TSX

TEST_F(MOS6502TXTYTests, TSX) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_TSX_IMP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	mem[0xFFFC] = ins;
	cpu.SP = 0x12;
	cpu.X = 0x52;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.X, cpu.SP);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502TXTYTests, TSXSetsZeroFlag) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_TSX_IMP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	mem[0xFFFC] = ins;
	cpu.SP = 0x0;
	cpu.X = 0xAB;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.X, cpu.SP);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502TXTYTests, TSXSetsNegativeFlag) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_TSX_IMP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	mem[0xFFFC] = ins;
	cpu.SP = 0xFF;
	cpu.X = 0xAB;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(cpu.X, cpu.SP);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// TXS

TEST_F(MOS6502TXTYTests, TXS) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_TXS_IMP;

	//Given:
	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();
	mem[0xFFFC] = ins;
	cpu.SP = 0x12;
	cpu.X = 0x52;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.X, cpu.SP);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// TXA
