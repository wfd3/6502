//
// Tests for and instruction for 6502 only
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

TEST_F(testClass, AndImmediate) {
	
	Byte ins = cpu.Opcodes.AND_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x0F;
	cpu.setA(0xff);

	//When:
	cpu.execute();

	// Then:
	EXPECT_EQ(cpu.getA(), 0x0F);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, AndImmediateSetsZeroFlag) {
	
	Byte ins = cpu.Opcodes.AND_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	cpu.setA(0xff);

	//When:
	cpu.execute();

	// Then:
	EXPECT_EQ(cpu.getA(), 0x00);
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, AndZeroPage) {
	
	Byte ins = cpu.Opcodes.AND_ZP;
	
	// Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	mem[0x0010] = 0x0F;
	cpu.setA(0xff); 

	// When: 
	cpu.execute();

	// Then
	EXPECT_EQ(cpu.getA(), 0x0f);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, AndZeroPageX) {
	
	Byte ins = cpu.Opcodes.AND_ZPX;
	
	// Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x01;
	cpu.setX(0x80);
	mem[0x81] = 0x0F;
	cpu.setA(0xFF);

	// When: 
	cpu.execute();

	// Then
	EXPECT_EQ(cpu.getA(), 0x0f);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, AndAbsolute) {
	
	Byte ins = cpu.Opcodes.AND_ABS;
	
	// Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x0F;
	cpu.setA(0xff);

	// When: 
	cpu.execute();

	// Then
	EXPECT_EQ(cpu.getA(), 0x0f);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, AndAbsoluteX) {
	
	Byte ins = cpu.Opcodes.AND_ABX;
	
	// Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setX(0x10);
	mem[0x2010] = 0x0F;
	cpu.setA(0xff);

	// When: 
	cpu.execute();

	// Then
	EXPECT_EQ(cpu.getA(), 0x0f);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, AndAbsoluteXCrossesPage) {
	
	Byte ins = cpu.Opcodes.AND_ABX;
	
	// Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xFF;
	mem[0xFFFE] = 0x20;
	cpu.setX(0x10);
	mem[0x20FF + 0x10] = 0x0F;
	cpu.setA(0xff); 

	// When: 
	cpu.execute();

	// Then
	EXPECT_EQ(cpu.getA(), 0x0f);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles());
}

TEST_F(testClass, AndAbsoluteY) {
	
	Byte ins = cpu.Opcodes.AND_ABY;
	
	// Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setY(0x10);
	mem[0x2010] = 0x0F;
	cpu.setA(0xff);

	// When: 
	cpu.execute();

	// Then
	EXPECT_EQ(cpu.getA(), 0x0f);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, AndAbsoluteYCrossesPage) {
	
	Byte ins = cpu.Opcodes.AND_ABY;
	
	// Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xFF;
	mem[0xFFFE] = 0x20;
	cpu.setY(0x10);
	mem[0x20FF + 0x10] = 0x0F;
	cpu.setA(0xff);

	// When: 
	cpu.execute();

	// Then
	EXPECT_EQ(cpu.getA(), 0x0f);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles());
}

TEST_F(testClass, AndIndirectX) {
	
	Byte ins = cpu.Opcodes.AND_IDX;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	cpu.setX(0x10);
	mem[0x0020] = 0x00;
	mem[0x0021] = 0x20;
	mem[0x2000] = 0x0F;
	cpu.setA(0xff);

	// When: 
	cpu.execute();

	// Then
	EXPECT_EQ(cpu.getA(), 0x0f);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles());
}

TEST_F(testClass, AndIndirectY) {
	
	Byte ins = cpu.Opcodes.AND_IDY;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	mem[0x0010] = 0x00;
	mem[0x0011] = 0x20;
	cpu.setY(0x10);
	mem[0x2000 + 0x10] = 0x0F;
	cpu.setA(0xff);

	// When: 
	cpu.execute();	

	// Then
	EXPECT_EQ(cpu.getA(), 0x0f);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles());

}
