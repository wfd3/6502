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
	
	Byte ins = cpu.Opcodes.TAX_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(0x52);
	cpu.setX(0xAB);

	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), cpu.getA());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, TAXSetsZeroFlag) {
	
	Byte ins = cpu.Opcodes.TAX_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(0);
	cpu.setX(0xab);

	//When:
	cpu.execute();

	// Then:
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), cpu.getA());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, TAXSetsNegativeFlag) {
	
	Byte ins = cpu.Opcodes.TAX_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(0xff);
	cpu.setX(0xab);

	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), cpu.getA());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

// TXA

TEST_F(testClass, TXA) {
	
	Byte ins = cpu.Opcodes.TXA_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(0x52);
	cpu.setX(0x0b);

	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), cpu.getA());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, TXASetsZeroFlag) {
	
	Byte ins = cpu.Opcodes.TXA_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setX(0);
	cpu.setA(0xab);

	//When:
	cpu.execute();

	// Then:
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), cpu.getA());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, TXASetsNegativeFlag) {
	
	Byte ins = cpu.Opcodes.TXA_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setX(0xff);
	cpu.setA(0xab);

	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), cpu.getA());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

// TAY

TEST_F(testClass, TAY) {
	
	Byte ins = cpu.Opcodes.TAY_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(0x52);
	cpu.setY(0x0b);

	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getY(), cpu.getA());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, TAYSetsZeroFlag) {
	
	Byte ins = cpu.Opcodes.TAY_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(0);
	cpu.setY(0xab);

	//When:
	cpu.execute();

	// Then:
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getY(), cpu.getA());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, TAYSetsNegativeFlag) {
	
	Byte ins = cpu.Opcodes.TAY_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(0xff);
	cpu.setY(0xab);

	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(cpu.getY(), cpu.getA());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

// TYA

TEST_F(testClass, TYA) {
	
	Byte ins = cpu.Opcodes.TYA_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(0x52);
	cpu.setY(0x0b);

	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getY(), cpu.getA());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, TYASetsZeroFlag) {
	
	Byte ins = cpu.Opcodes.TYA_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setY(0);
	cpu.setA(0xab);

	//When:
	cpu.execute();

	// Then:
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getY(), cpu.getA());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, TYASetsNegativeFlag) {
	
	Byte ins = cpu.Opcodes.TYA_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setY(0xff);
	cpu.setA(0xab);

	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(cpu.getY(), cpu.getA());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

// TSX

TEST_F(testClass, TSX) {
	
	Byte ins = cpu.Opcodes.TSX_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR, 0x12);
	
	mem[0xFFFC] = ins;
	cpu.setX(0x52);

	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), cpu.getSP());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, TSXSetsZeroFlag) {
	
	Byte ins = cpu.Opcodes.TSX_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR, 0);
	
	mem[0xFFFC] = ins;
	cpu.setX(0xab);

	//When:
	cpu.execute();

	// Then:
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), cpu.getSP());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, TSXSetsNegativeFlag) {
	
	Byte ins = cpu.Opcodes.TSX_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR, 0xff);
	
	mem[0xFFFC] = ins;
	cpu.setX(0xab);

	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), cpu.getSP());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

// TXS

TEST_F(testClass, TXS) {
	
	Byte ins = cpu.Opcodes.TXS_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR, 0x12);
	
	mem[0xFFFC] = ins;
	cpu.setX(0x52);

	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), cpu.getSP());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}
