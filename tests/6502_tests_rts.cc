#include <gtest/gtest.h>
#include <6502.h>

class MOS6502RTSTests : public testing::Test {
public:

	Memory<Address_t, Byte> mem{CPU::MAX_MEM};
	CPU cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, CPU::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}
};

TEST_F(MOS6502RTSTests, RtsImplied) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_RTS_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR, CPU::INITIAL_SP - 2);
	mem[0x01FF] = 0x20;
	mem[0x01FE] = 0x00;

	mem[0xFFFC] = ins;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.PC, 0x2000 + 1);
	EXPECT_EQ(cpu.SP, CPU::INITIAL_SP);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
