//
// Tests for flag instructions (clc, cld, cli, clv, sec, sed, sei)
//
// Copyright (C) 2023 Walt Drummond
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.

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
	cpu.setFlagC(true);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
}

TEST_F(MOS6502FlagTests, SECSetsCarryFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_SEC_IMP;

	cpu.TestReset(CPU::RESET_VECTOR);

	// Given:
	mem[0xFFFC] = ins;
	cpu.setFlagC(false);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
}

TEST_F(MOS6502FlagTests, CLDClearsDecimalFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_CLD_IMP;

	cpu.TestReset(CPU::RESET_VECTOR);

	// Given:
	mem[0xFFFC] = ins;
	cpu.setFlagD(true);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagD());
}

TEST_F(MOS6502FlagTests, SEDSetsDecimalFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_SED_IMP;

	cpu.TestReset(CPU::RESET_VECTOR);

	// Given:
	mem[0xFFFC] = ins;
	cpu.setFlagD(false);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagD());
}

TEST_F(MOS6502FlagTests, CLIClearsInterruptFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_CLI_IMP;

	cpu.TestReset(CPU::RESET_VECTOR);

	// Given:
	mem[0xFFFC] = ins;
	cpu.setFlagI(true);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagI());
}

TEST_F(MOS6502FlagTests, SEISetsInterruptFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_SEI_IMP;

	cpu.TestReset(CPU::RESET_VECTOR);
	
	// Given:
	mem[0xFFFC] = ins;
	cpu.setFlagI(false);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagI());
}

TEST_F(MOS6502FlagTests, CLVClearsOverflowFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_CLV_IMP;

	cpu.TestReset(CPU::RESET_VECTOR);

	// Given:
	mem[0xFFFC] = ins;
	cpu.setFlagV(true);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagV());
}
