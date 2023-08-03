#include <gtest/gtest.h>
#include "../6502.h"

class MOS6502BRKTests : public testing::Test {
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

TEST_F(MOS6502BRKTests, BRKImplied) {
	CPU::Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_BRK_IMP;

	//Given:
	cpu.Reset(CPU::INITIAL_PC);
	mem[0xFFFC] = ins;
	mem[0xFFFE] = 0x00;
	mem[0xFFFF] = 0x60;

	//When:
	std::tie(UsedCycles, ExpectedCycles) = cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.PC, 0x6000);
	EXPECT_EQ(cpu.SP, CPU::INITIAL_SP - 3);
	EXPECT_TRUE(cpu.Flags.B);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
