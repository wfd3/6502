//
// Tests increment instructions for 6502 only
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

// INC 
TEST_F(testClass, IncZeroPage) {
	
	Byte ins = cpu.Opcodes.INC_ZP;

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
	EXPECT_EQ(mem[0x0020], 0x03);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, IncZeroPageX) {
	
	Byte ins = cpu.Opcodes.INC_ZPX;

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
	EXPECT_EQ(mem[0x0040], 0x03);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, IncAbsolute) {
	
	Byte ins = cpu.Opcodes.INC_ABS;

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
	EXPECT_EQ(mem[0x2000], 0x03);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, IncAbsoluteX) {
	
	Byte ins = cpu.Opcodes.INC_ABX;

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
	EXPECT_EQ(mem[0x2002], 0x03);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, IncZeroPageSetsZeroFlag) {
	
	Byte ins = cpu.Opcodes.INC_ZP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0x0020] = 0xff;

	//When:
	cpu.execute();

	// Then:
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(mem[0x0020], 0x00);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, IncZeroPageSetsNegativeFlag) {
	
	Byte ins = cpu.Opcodes.INC_ZP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0x0020] = 0x7f;

	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(mem[0x0020], 0x80);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

// INX
TEST_F(testClass, InxImplied) {
	
	Byte ins = cpu.Opcodes.INX_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	cpu.setX(0x09);
	
	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), 0x0a);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, InxImpliedSetsZeroFlag) {
	
	Byte ins = cpu.Opcodes.INX_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	cpu.setX(0xff);
	
	//When:
	cpu.execute();

	// Then:
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), 0);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, InxImpliedSetsNegativeFlag) {
	
	Byte ins = cpu.Opcodes.INX_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	cpu.setX(127);
	
	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), 128);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

// INY
TEST_F(testClass, InyImplied) {
	
	Byte ins = cpu.Opcodes.INY_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	cpu.setY(0x09);
	
	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getY(), 0x0a);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, InyImpliedSetsZeroFlag) {
	
	Byte ins = cpu.Opcodes.INY_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setY(0xff);
	
	//When:
	cpu.execute();

	// Then:
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getY(), 0);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, InyImpliedSetsNegativeFlag) {
	
	Byte ins = cpu.Opcodes.INY_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	cpu.setY(127);
	
	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(cpu.getY(), 128);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}
