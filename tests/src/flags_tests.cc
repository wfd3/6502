//
// Tests flags for 6502 only
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

#if !defined(testClass) 
# error "Macro 'testClass' not defined"
#endif

TEST_F(testClass, CLCClearsCarryFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.CLC_IMP;

	cpu.TestReset(MOS6502::RESET_VECTOR);

	// Given:
	mem[0xFFFC] = ins;
	cpu.setFlagC(true);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
}

TEST_F(testClass, SECSetsCarryFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.SEC_IMP;

	cpu.TestReset(MOS6502::RESET_VECTOR);

	// Given:
	mem[0xFFFC] = ins;
	cpu.setFlagC(false);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
}

TEST_F(testClass, CLDClearsDecimalFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.CLD_IMP;

	cpu.TestReset(MOS6502::RESET_VECTOR);

	// Given:
	mem[0xFFFC] = ins;
	cpu.setFlagD(true);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagD());
}

TEST_F(testClass, SEDSetsDecimalFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.SED_IMP;

	cpu.TestReset(MOS6502::RESET_VECTOR);

	// Given:
	mem[0xFFFC] = ins;
	cpu.setFlagD(false);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagD());
}

TEST_F(testClass, CLIClearsInterruptFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.CLI_IMP;

	cpu.TestReset(MOS6502::RESET_VECTOR);

	// Given:
	mem[0xFFFC] = ins;
	cpu.setFlagI(true);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagI());
}

TEST_F(testClass, SEISetsInterruptFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.SEI_IMP;

	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	// Given:
	mem[0xFFFC] = ins;
	cpu.setFlagI(false);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagI());
}

TEST_F(testClass, CLVClearsOverflowFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.CLV_IMP;

	cpu.TestReset(MOS6502::RESET_VECTOR);

	// Given:
	mem[0xFFFC] = ins;
	cpu.setFlagV(true);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagV());
}
