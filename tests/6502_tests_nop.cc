#include <gtest/gtest.h>
#include "../6502.h"

class MOS6502NOPTests : public testing::Test {
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

TEST_F(MOS6502NOPTests, Nop) {
	CPU::Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_NOP_IMP;

	//Given:
	cpu.Reset(CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(UsedCycles, ExpectedCycles);
}
