#include <gtest/gtest.h>
#include <6502.h>

class MOS6502RTITests : public testing::Test {
public:

	Memory<Address_t, Byte> mem{CPU::MAX_MEM};
	CPU cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, CPU::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}
};

TEST_F(MOS6502RTITests, RtiImplied) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_RTI_IMP;

	//Given:
	cpu.TestReset(0x2000, CPU::INITIAL_SP - 3);
	mem[0x2000] = ins;
	mem[0x01FF] = 0xAA;
	mem[0x01FE] = 0xFF;
	mem[0x01FD] = 0x00;
	cpu.SP -= 3;
	cpu.PS = 0xff;
	cpu.Flags.B = 1;
	cpu.Flags.C = 1;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.PC, 0xAAFF);
	EXPECT_EQ(cpu.SP, CPU::INITIAL_SP);
	EXPECT_FALSE(cpu.Flags.B);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
