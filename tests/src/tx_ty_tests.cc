//
// Tests for the TAX & TAY instruction for 6502 only
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

// TAX

TEST_F(testClass, TAX) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.TAX_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(0x52);
	cpu.setX(0xAB);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), cpu.getA());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, TAXSetsZeroFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.TAX_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(0);
	cpu.setX(0xab);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), cpu.getA());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, TAXSetsNegativeFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.TAX_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(0xff);
	cpu.setX(0xab);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), cpu.getA());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// TXA

TEST_F(testClass, TXA) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.TXA_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(0x52);
	cpu.setX(0x0b);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), cpu.getA());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, TXASetsZeroFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.TXA_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setX(0);
	cpu.setA(0xab);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), cpu.getA());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, TXASetsNegativeFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.TXA_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setX(0xff);
	cpu.setA(0xab);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), cpu.getA());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// TAY

TEST_F(testClass, TAY) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.TAY_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(0x52);
	cpu.setY(0x0b);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getY(), cpu.getA());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, TAYSetsZeroFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.TAY_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(0);
	cpu.setY(0xab);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getY(), cpu.getA());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, TAYSetsNegativeFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.TAY_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(0xff);
	cpu.setY(0xab);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(cpu.getY(), cpu.getA());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// TYA

TEST_F(testClass, TYA) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.TYA_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(0x52);
	cpu.setY(0x0b);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getY(), cpu.getA());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, TYASetsZeroFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.TYA_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setY(0);
	cpu.setA(0xab);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getY(), cpu.getA());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, TYASetsNegativeFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.TYA_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setY(0xff);
	cpu.setA(0xab);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(cpu.getY(), cpu.getA());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// TSX

TEST_F(testClass, TSX) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.TSX_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR, 0x12);
	
	mem[0xFFFC] = ins;
	cpu.setX(0x52);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), cpu.getSP());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, TSXSetsZeroFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.TSX_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR, 0);
	
	mem[0xFFFC] = ins;
	cpu.setX(0xab);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), cpu.getSP());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, TSXSetsNegativeFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.TSX_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR, 0xff);
	
	mem[0xFFFC] = ins;
	cpu.setX(0xab);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), cpu.getSP());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// TXS

TEST_F(testClass, TXS) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.TXS_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR, 0x12);
	
	mem[0xFFFC] = ins;
	cpu.setX(0x52);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), cpu.getSP());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
