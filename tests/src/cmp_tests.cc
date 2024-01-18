//
// Tests for brk instruction for 6502 only
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

TEST_F(testClass, CmpImmediate) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.CMP_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	cpu.setA(0x30);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, CmpImmediateSetsCFlagFalse) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.CMP_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xFF;
	cpu.setA(0x0f);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, CmpImmediateSetsZFlagTrue) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.CMP_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xFF;
	cpu.setA(0xff);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, CmpImmediateSetsNFlagTrue) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.CMP_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	cpu.setA(0x10);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, CmpZeroPage) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.CMP_ZP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0x0000] = 0x0F;
	cpu.setA(0x20);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, CmpZeroPageX) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.CMP_ZPX;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	cpu.setX(0x01);
	mem[0x0001] = 0x0F;
	cpu.setA(0x2f);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, CmpAbsolute) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.CMP_ABS;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x0F;
	cpu.setA(0x2f);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, CmpAbsoluteX) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.CMP_ABX;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setX(0x01);
	mem[0x2001] = 0x0F;
	cpu.setA(0x2f);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, CmpAbsoluteXCrossesPage) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.CMP_ABX;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	mem[0xFFFE] = 0x20;
	cpu.setX(0xff);
	mem[0x210f] = 0x0F;
	cpu.setA(0x2f);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, CmpAbsoluteY) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.CMP_ABY;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setY(0x01);
	mem[0x2001] = 0x0F;
	cpu.setA(0x2f);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, CmpAbsoluteYCrossesPage) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.CMP_ABY;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	mem[0xFFFE] = 0x20;
	cpu.setY(0xff);
	mem[0x210f] = 0x0F;
	cpu.setA(0x2f);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, CmpIndirectX) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.CMP_IDX;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x01;
	cpu.setX(0x01);
	mem[0x0002] = 0x01;
	mem[0x0000] = 0x20;
	mem[0x2001] = 0x0F;
	cpu.setA(0x2f);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, CmpIndirectY) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.CMP_IDY;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x02;
	cpu.setY(0x01);
	mem[0x0002] = 0x01;
	mem[0x0000] = 0x20;
	mem[0x2002] = 0x0F;
	cpu.setA(0x2f);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, CmpIndirectYCrossesPage) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.CMP_IDY;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x02;
	cpu.setY(0xff);
	mem[0x0002] = 0x10;
	mem[0x0000] = 0x20;
	mem[0x210F] = 0x0F;
	cpu.setA(0x2f);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// CPX
TEST_F(testClass, CpxImmediate) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.CPX_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	cpu.setX(0x30);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, CpxImmediateSetsCFlagFalse) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.CPX_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xf;
	cpu.setX(0x0e);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, CpxImmediateSetsZFlagTrue) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.CPX_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xFF;
	cpu.setX(0xff);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, CpxImmediateSetsNFlagTrue) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.CPX_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x2F;
	cpu.setX(0x1f);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, CpxZeroPage) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.CPX_ZP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0x0000] = 0x0F;
	cpu.setX(0x2f);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, CpxAbsolute) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.CPX_ABS;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x0F;
	cpu.setX(0x2f);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// CPY
TEST_F(testClass, CpyImmediate) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.CPY_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	cpu.setY(0x30);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, CpyImmediateSetsCFlagFalse) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.CPY_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xf;
	cpu.setY(0x0e);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, CpyImmediateSetsZFlagTrue) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.CPY_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xFF;
	cpu.setY(0xff);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, CpyImmediateSetsNFlagTrue) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.CPY_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x3F;
	cpu.setY(0x2f);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, CpyZeroPage) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.CPY_ZP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0x0000] = 0x0F;
	cpu.setY(0x2f);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, CpyAbsolute) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.CPY_ABS;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x0F;
	cpu.setY(0x2f);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
