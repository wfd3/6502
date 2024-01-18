//
// Tests for asl instruction for 6502 only
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

TEST_F(testClass, AslAccumulator) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.ASL_ACC;
	Byte data = 0b01010101;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	cpu.setA(data); 

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_EQ(cpu.getA(), data << 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, AslAccumulatorSetsCarryFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.ASL_ACC;
	Byte data = 0b10000001;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	cpu.setA(data);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_EQ(cpu.getA(), Byte(data << 1));
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, AslAccumulatorSetsNegativeFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.ASL_ACC;
	Byte data = 0b01000001;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	cpu.setA(data);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_EQ(cpu.getA(), Byte(data << 1));
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, AslAccumulatorSetsZeroFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.ASL_ACC;
	Byte data = 0b00000000;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	cpu.setA(data);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_EQ(cpu.getA(), Byte(data << 1));
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, AslZeroPage) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.ASL_ZP;
	Byte data = 0b01010101;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x0001;
	mem[0x0001] = data;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_EQ(mem[0x0001], data << 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, AslZeroPageX) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.ASL_ZPX;
	Byte data = 0b01010101;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x0001;
	cpu.setX(0x02);
	mem[0x0003] = data;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_EQ(mem[0x0003], data << 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, AslAbsolute) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.ASL_ABX;
	Byte data = 0b01010101;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = data;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_EQ(mem[0x2000], data << 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, AslAbsoluteX) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.ASL_ABX;
	Byte data = 0b01010101;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setX(0x02);
	mem[0x2002] = data;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_EQ(mem[0x2002], data << 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
