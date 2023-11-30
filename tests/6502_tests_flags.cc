#include <gtest/gtest.h>
#include <6502.h>

class MOS6502FlagTests : public testing::Test {
public:

	Memory<Address_t, Byte> mem{CPU::MAX_MEM};
	CPU cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, CPU::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}
};

TEST_F(MOS6502FlagTests, CLCClearsCarryFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_CLC_IMP;

	cpu.TestReset(CPU::RESET_VECTOR);
	

	// Given:
	mem[0xFFFC] = ins;
	cpu.Flags.C = 1;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.Flags.C , 0);
}

TEST_F(MOS6502FlagTests, SECSetsCarryFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_SEC_IMP;

	cpu.TestReset(CPU::RESET_VECTOR);
	

	// Given:
	mem[0xFFFC] = ins;
	cpu.Flags.C = 0;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.Flags.C , 1);
}

TEST_F(MOS6502FlagTests, CLDClearsDecimalFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_CLD_IMP;

	cpu.TestReset(CPU::RESET_VECTOR);
	

	// Given:
	mem[0xFFFC] = ins;
	cpu.Flags.D = 1;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.Flags.D , 0);
}

TEST_F(MOS6502FlagTests, SEDSetsDecimalFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_SED_IMP;

	cpu.TestReset(CPU::RESET_VECTOR);
	

	// Given:
	mem[0xFFFC] = ins;
	cpu.Flags.D = 0;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.Flags.D , 1);
}

TEST_F(MOS6502FlagTests, CLIClearsInterruptFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_CLI_IMP;

	cpu.TestReset(CPU::RESET_VECTOR);
	

	// Given:
	mem[0xFFFC] = ins;
	cpu.Flags.I = 1;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.Flags.I , 0);
}

TEST_F(MOS6502FlagTests, SEISetsInterruptFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_SEI_IMP;

	cpu.TestReset(CPU::RESET_VECTOR);
	

	// Given:
	mem[0xFFFC] = ins;
	cpu.Flags.I = 0;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.Flags.I , 1);
}


TEST_F(MOS6502FlagTests, CLVClearsOverflowFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_CLV_IMP;

	cpu.TestReset(CPU::RESET_VECTOR);
	

	// Given:
	mem[0xFFFC] = ins;
	cpu.Flags.V = 1;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.Flags.V , 0);
}
