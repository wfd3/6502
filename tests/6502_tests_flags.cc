#include <gtest/gtest.h>
#include "../6502.h"

class MOS6502FlagTests : public testing::Test {
public:

	Memory<Address_t, Byte> mem{CPU::MAX_MEM};
	CPU cpu{&mem};

	virtual void SetUp() {
		cpu.exitReset();
		mem.mapRAM(0, CPU::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}
};

TEST_F(MOS6502FlagTests, CLCClearsCarryFlag) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_CLC_IMP;

	cpu.Reset(CPU::RESET_VECTOR);
	

	// Given:
	mem[0xFFFC] = ins;
	cpu.Flags.C = 1;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.Flags.C , 0);
}

TEST_F(MOS6502FlagTests, SECSetsCarryFlag) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_SEC_IMP;

	cpu.Reset(CPU::RESET_VECTOR);
	

	// Given:
	mem[0xFFFC] = ins;
	cpu.Flags.C = 0;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.Flags.C , 1);
}

TEST_F(MOS6502FlagTests, CLDClearsDecimalFlag) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_CLD_IMP;

	cpu.Reset(CPU::RESET_VECTOR);
	

	// Given:
	mem[0xFFFC] = ins;
	cpu.Flags.D = 1;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.Flags.D , 0);
}

TEST_F(MOS6502FlagTests, SEDSetsDecimalFlag) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_SED_IMP;

	cpu.Reset(CPU::RESET_VECTOR);
	

	// Given:
	mem[0xFFFC] = ins;
	cpu.Flags.D = 0;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.Flags.D , 1);
}

TEST_F(MOS6502FlagTests, CLIClearsInterruptFlag) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_CLI_IMP;

	cpu.Reset(CPU::RESET_VECTOR);
	

	// Given:
	mem[0xFFFC] = ins;
	cpu.Flags.I = 1;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.Flags.I , 0);
}

TEST_F(MOS6502FlagTests, SEISetsInterruptFlag) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_SEI_IMP;

	cpu.Reset(CPU::RESET_VECTOR);
	

	// Given:
	mem[0xFFFC] = ins;
	cpu.Flags.I = 0;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.Flags.I , 1);
}


TEST_F(MOS6502FlagTests, CLVClearsOverflowFlag) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_CLV_IMP;

	cpu.Reset(CPU::RESET_VECTOR);
	

	// Given:
	mem[0xFFFC] = ins;
	cpu.Flags.V = 1;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.Flags.V , 0);
}

