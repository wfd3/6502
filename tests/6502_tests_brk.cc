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
	Word pushed_PC = CPU::INITIAL_PC + 2;
	constexpr Word STACK_FRAME = 0x0100 | CPU::INITIAL_SP;

	//Given:
	cpu.Reset(CPU::INITIAL_PC);
	mem[0xFFFC] = ins;
	mem[0xFFFE] = 0x00;
	mem[0xFFFF] = 0x60;
	cpu.PS = 1 << 5;

	//When:
	std::tie(UsedCycles, ExpectedCycles) = cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.PC, 0x6000);
	EXPECT_EQ(cpu.SP, CPU::INITIAL_SP - 3);
	EXPECT_EQ(mem[STACK_FRAME], pushed_PC & 0xff);
	EXPECT_EQ(mem[STACK_FRAME-1], pushed_PC >> 8);
	EXPECT_EQ(mem[STACK_FRAME-2], 1 << 5);
	EXPECT_TRUE(cpu.Flags.B);
	EXPECT_TRUE(cpu.Flags.I);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
