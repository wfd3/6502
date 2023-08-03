#include <gtest/gtest.h>
#include "../6502.h"

class MOS6502RTSTests : public testing::Test {
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

TEST_F(MOS6502RTSTests, RtsImplied) {
	CPU::Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_RTS_IMP;

	//Given:
	cpu.Reset(CPU::INITIAL_PC);
	mem[0x01FF] = 0x00;
	mem[0x01FE] = 0x20;
	cpu.SP -= 2;

	mem[0xFFFC] = ins;

	//When:
	std::tie(UsedCycles, ExpectedCycles) = cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.PC, 0x2000);
	EXPECT_EQ(cpu.SP, CPU::INITIAL_SP);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}