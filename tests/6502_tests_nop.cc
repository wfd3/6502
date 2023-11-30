#include <gtest/gtest.h>
#include <6502.h>

class MOS6502NOPTests : public testing::Test {
public:

	Memory<Address_t, Byte> mem{CPU::MAX_MEM};
	CPU cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, CPU::MAX_MEM);
	}

	virtual void TearDown()	{
	}
};

TEST_F(MOS6502NOPTests, Nop) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_NOP_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(UsedCycles, ExpectedCycles);
}
