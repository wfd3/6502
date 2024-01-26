//
// Tests for eor instruction for 6502 only
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

TEST_F(testClass, EorImmediate) {
	
	Byte ins = cpu.Opcodes.EOR_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x01;
	cpu.setA(0);

	//When:
	cpu.execute();

	// Then:
	EXPECT_EQ(cpu.getA(), 0x01);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN()) ;
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, EorZeroPage) {
	
	Byte ins = cpu.Opcodes.EOR_ZP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x01;
	mem[0x0001] = 0x01;
	cpu.setA(0);

	//When:
	cpu.execute();

	// Then:
	EXPECT_EQ(cpu.getA(), 0x01);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN()) ;
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, EorZeroPageX) {
	
	Byte ins = cpu.Opcodes.EOR_ZPX;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x01;
	cpu.setX(1);
	mem[0x0002] = 0x01;
	cpu.setA(0);

	//When:
	cpu.execute();

	// Then:
	EXPECT_EQ(cpu.getA(), 0x01);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN()) ;
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, EorAbsolute) {
	
	Byte ins = cpu.Opcodes.EOR_ABS;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x01;
	cpu.setA(0);

	//When:
	cpu.execute();

	// Then:
	EXPECT_EQ(cpu.getA(), 0x01);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN()) ;
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, EorAbsoluteX) {
	
	Byte ins = cpu.Opcodes.EOR_ABX;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setX(1);
	mem[0x2001] = 0x01;
	cpu.setA(0);

	//When:
	cpu.execute();

	// Then:
	EXPECT_EQ(cpu.getA(), 0x01);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN()) ;
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, EorAbsoluteY) {
	
	Byte ins = cpu.Opcodes.EOR_ABY;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setY(1);
	mem[0x2001] = 0x01;
	cpu.setA(0);

	//When:
	cpu.execute();

	// Then:
	EXPECT_EQ(cpu.getA(), 0x01);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN()) ;
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, EorIndirectX) {
	
	Byte ins = cpu.Opcodes.EOR_IDX;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	cpu.setX(0x10);
	mem[0x0020] = 0x00;
	mem[0x0021] = 0x20;
	mem[0x2000] = 0x01;
	cpu.setA(0);

	//When:
	cpu.execute();

	// Then:
	EXPECT_EQ(cpu.getA(), 0x01);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN()) ;
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, EorIndirectY) {
	
	Byte ins = cpu.Opcodes.EOR_IDY;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;

	mem[0x0020] = 0x00;
	mem[0x0021] = 0x20;
	cpu.setY(1);
	mem[0x2001] = 0x01;
	cpu.setA(0);

	//When:
	cpu.execute();

	// Then:
	EXPECT_EQ(cpu.getA(), 0x01);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN()) ;
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, EorImmediateWhenZeroFlagShouldBeSet) {
	
	Byte ins = cpu.Opcodes.EOR_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x0F;
	cpu.setA(0x0f);

	//When:
	cpu.execute();

	// Then:
	EXPECT_EQ(cpu.getA(), 0x00);
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN()) ;
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, EorImmediateWhenNegativeFlagShouldBeSet) {
	
	Byte ins = cpu.Opcodes.EOR_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x0F;
	cpu.setA(0xf0);

	//When:
	cpu.execute();

	// Then:
	EXPECT_EQ(cpu.getA(), 0xFF);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN()) ;
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

