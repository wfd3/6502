//
// Tests for decrement instructions for 6502 only
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

// DEC 
TEST_F(testClass, DecZeroPage) {
	
	Byte ins = cpu.Opcodes.DEC_ZP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0x0020] = 0x02;

	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(mem[0x0020], 0x01);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, DecZeroPageX) {
	
	Byte ins = cpu.Opcodes.DEC_ZPX;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	cpu.setX(0x20);
	mem[0x0040] = 0x02;

	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(mem[0x0040], 0x01);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, DecAbsolute) {
	
	Byte ins = cpu.Opcodes.DEC_ABS;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x02;

	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(mem[0x2000], 0x01);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, DecAbsoluteX) {
	
	Byte ins = cpu.Opcodes.DEC_ABX;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setX(0x02);
	mem[0x2002] = 0x02;

	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(mem[0x2002], 0x01);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, DecZeroPageSetsZeroFlag) {
	
	Byte ins = cpu.Opcodes.DEC_ZP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0x0020] = 0x01;

	//When:
	cpu.execute();

	// Then:
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(mem[0x0020], 0x00);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, DecZeroPageSetsNegativeFlag) {
	
	Byte ins = cpu.Opcodes.DEC_ZP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0x0020] = 0x00;

	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(mem[0x0020], 0xff);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

// DEX
TEST_F(testClass, DexImmediate) {
	
	Byte ins = cpu.Opcodes.DEX_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setX(10);
	
	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), 9);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, DexImmediateSetsZeroFlag) {
	
	Byte ins = cpu.Opcodes.DEX_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setX(1);
	
	//When:
	cpu.execute();

	// Then:
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), 0);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, DexImmediateSetsNegativeFlag) {
	
	Byte ins = cpu.Opcodes.DEX_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setX(0);
	
	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), 0xff);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

// DEY
TEST_F(testClass, DeyImmediate) {
	
	Byte ins = cpu.Opcodes.DEY_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setY(10);
	
	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getY(), 9);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, DeyImmediateSetsZeroFlag) {
	
	Byte ins = cpu.Opcodes.DEY_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setY(1);
	
	//When:
	cpu.execute();

	// Then:
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getY(), 0);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, DeyImmediateSetsNegativeFlag) {
	
	Byte ins = cpu.Opcodes.DEY_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	cpu.setY(0);
	
	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(cpu.getY(), 0xff);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}
