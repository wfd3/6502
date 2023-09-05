#include <gtest/gtest.h>
#include <6502.h>

class MOS6502RTSTests : public testing::Test {
public:

	Memory<Address_t, Byte> mem{CPU::MAX_MEM};
	CPU cpu{mem};

	virtual void SetUp() {
		cpu.exitReset();
		mem.mapRAM(0, CPU::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}
};

TEST_F(MOS6502RTSTests, RtsImplied) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_RTS_IMP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	mem[0x01FF] = 0x20;
	mem[0x01FE] = 0x00;
	cpu.SP -= 2;

	mem[0xFFFC] = ins;

	//When:
	std::tie(UsedCycles, ExpectedCycles) = cpu.executeOneInstruction();

	// Then:
	EXPECT_EQ(cpu.PC, 0x2000 + 1);
	EXPECT_EQ(cpu.SP, CPU::INITIAL_SP);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
