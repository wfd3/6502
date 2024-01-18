//
// Tests LSR instruction
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

TEST_F(testClass, LsrAccumulator) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.LSR_ACC;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	cpu.setA(0b01010101);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getA(), 0b00101010);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, LsrZeroPage) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.LSR_ZP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0x0020] = 0b01010101;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(mem[0x0020], 0b00101010);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, LsrZeroPageX) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.LSR_ZPX;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	cpu.setX(0x10);
	mem[0x0020] = 0b01010101;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(mem[0x0020], 0b00101010);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, LsrAbsolute) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.LSR_ABS;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0b01010101;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(mem[0x2000], 0b00101010);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, LsrAbsoluteX) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.LSR_ABX;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setX(0x05);
	mem[0x2005] = 0b01010101;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(mem[0x2005], 0b00101010);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

