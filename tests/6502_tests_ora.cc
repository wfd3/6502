#include <gtest/gtest.h>
#include "../6502.h"

class MOS6502ORATests : public testing::Test {
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

TEST_F(MOS6502ORATests, OraImmediateWhenNegativeFlagShouldNotBeSet) {
	CPU::Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ORA_IMM;

	//Given:
	cpu.Reset(CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x0F;
	cpu.A = 0x0F;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, 0x0F);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}


TEST_F(MOS6502ORATests, OraImmediateWhenNegativeFlagShouldBeSet) {
	CPU::Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ORA_IMM;

	//Given:
	cpu.Reset(CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x0F;
	cpu.A = 0xF0;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, 0xFF);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

