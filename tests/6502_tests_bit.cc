#include <gtest/gtest.h>
#include "../6502.h"

class MOS6502BITTests : public testing::Test {
public:
	Memory mem{CPU::MAX_MEM};
	CPU cpu{&mem};


	virtual void SetUp() {
		cpu.exitReset();
		mem.Init();
	}

	virtual void TearDown()	{
	}
};

TEST_F(MOS6502BITTests, BitAbsolute) {
	CPU::Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_BIT_ABS;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
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
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502BITTests, BitZeroPage) {
	CPU::Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_BIT_ZP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x01;
	mem[0x0001] = 0x0F;
	cpu.A = 0xFF;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502BITTests, BitZeroPageSetsZeroFlag) {
	CPU::Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_BIT_ZP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x01;
	mem[0x0001] = 0xF;
	cpu.A = 0x00;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502BITTests, BitZeroPageSetsOverflowFlag) {
	CPU::Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_BIT_ZP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x01;
	mem[0x0001] = 0x4F;
	cpu.A = 1 << 6;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502BITTests, BitZeroPageSetsNegativeFlag) {
	CPU::Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_BIT_ZP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x01;
	mem[0x0001] = 0x8F;
	cpu.A = 1 << 7;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}


