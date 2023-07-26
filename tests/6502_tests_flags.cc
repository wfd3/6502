#include <gtest/gtest.h>
#include "../6502.h"

extern mos6502::MEMORY mem;
extern mos6502::CPU cpu;

class MOS6502FlagTests : public testing::Test {
public:

	virtual void SetUp() {
	}
	
	virtual void TearDown()	{
	}
};

TEST_F(MOS6502FlagTests, CLCClearsCarryFlag) {
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_CLC_IMP;

	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

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
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_SEC_IMP;

	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

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
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_CLD_IMP;

	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

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
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_SED_IMP;

	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

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
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_CLI_IMP;

	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

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
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_SEI_IMP;

	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

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
	mos6502::Cycles_t UsedCycles, ExpectedCycles;
	mos6502::Byte ins = mos6502::CPU::INS_CLV_IMP;

	cpu.Reset(mos6502::CPU::INITIAL_PC);
	mem.Init();

	// Given:
	mem[0xFFFC] = ins;
	cpu.Flags.V = 1;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.Flags.V , 0);
}

