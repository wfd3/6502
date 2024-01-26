//
// Tests the ROL & ROR instructions
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

TEST_F(testClass, RolAccumulator) {
	
	Byte ins = cpu.Opcodes.ROL_ACC;
	Byte data = 0b01010101;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(data);

	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(cpu.getA(), data << 1);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, RolAccumulatorSetsCarryFlag) {
	
	Byte ins = cpu.Opcodes.ROL_ACC;
	Byte data = 0b10000001;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(data); 

	//When:
	cpu.execute();

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getA(), Byte(data << 1));
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, RolZeroPage) {
	
	Byte ins = cpu.Opcodes.ROL_ZP;
	Byte data = 0b01010101;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0x0020] = data;

	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(mem[0x0020], data << 1);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, RolZeroPageX) {
	
	Byte ins = cpu.Opcodes.ROL_ZPX;
	Byte data = 0b01010101;
	
	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	cpu.setX(0x10);
	mem[0x0020] = data;

	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(mem[0x0020], data << 1);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, RolAbsolute) {
	
	Byte ins = cpu.Opcodes.ROL_ABS;
	Byte data = 0b01010101;
	
	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = data;

	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(mem[0x2000], data << 1);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, RolAbsoluteX) {
	
	Byte ins = cpu.Opcodes.ROL_ABX;
	Byte data = 0b01010101;
	
	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setX(0x05);
	mem[0x2005] = data;

	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(mem[0x2005], Byte(data << 1));
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

// ROR
TEST_F(testClass, RorAccumulator) {
	
	Byte ins = cpu.Opcodes.ROR_ACC;
	Byte data = 0b01010101;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(data); 

	//When:
	cpu.execute();

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getA(), data >> 1);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, RorAccumulatorSetsCarryFlag) {
	
	Byte ins = cpu.Opcodes.ROR_ACC;
	Byte data = 0b10000001;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(data); 

	//When:
	cpu.execute();

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getA(), Byte(data >> 1));
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, RorAccumulatorClearsCarryAndSetsNegativeFlag) {
	
	Byte ins = cpu.Opcodes.ROR_ACC;
	Byte data = 0b10000000;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	cpu.setA(data); 
	cpu.setFlagC(true);

	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(cpu.getA(), Byte(data >> 1) | (1 << 7));
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, RorZeroPage) {
	
	Byte ins = cpu.Opcodes.ROR_ZP;
	Byte data = 0b01010101;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0x0020] = data;

	//When:
	cpu.execute();

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(mem[0x0020], data >> 1);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, RorZeroPageX) {
	
	Byte ins = cpu.Opcodes.ROR_ZPX;
	Byte data = 0b01010101;
	
	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	cpu.setX(0x10);
	mem[0x0020] = data;

	//When:
	cpu.execute();

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(mem[0x0020], data >> 1);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, RorAbsolute) {
	
	Byte ins = cpu.Opcodes.ROR_ABS;
	Byte data = 0b01010101;
	
	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = data;

	//When:
	cpu.execute();

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(mem[0x2000], data >> 1);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, RorAbsoluteX) {
	
	Byte ins = cpu.Opcodes.ROR_ABX;
	Byte data = 0b01010101;
	
	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setX(0x05);
	mem[0x2005] = data;

	//When:
	cpu.execute();

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(mem[0x2005], Byte(data >> 1));
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}
