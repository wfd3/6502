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

TEST_F(testClass, BitAbsolute) {
	
	Byte ins = cpu.Opcodes.BIT_ABS;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x0F;
	cpu.setA(0xff);

	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, BitZeroPage) {
	
	Byte ins = cpu.Opcodes.BIT_ZP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x01;
	mem[0x0001] = 0x0F;
	cpu.setA(0xff);

	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, BitZeroPageSetsZeroFlag) {
	
	Byte ins = cpu.Opcodes.BIT_ZP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x01;
	mem[0x0001] = 0xF;
	cpu.setA(0);

	//When:
	cpu.execute();

	// Then:
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, BitZeroPageSetsOverflowFlag) {
	
	Byte ins = cpu.Opcodes.BIT_ZP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x01;
	mem[0x0001] = 0x4F;
	cpu.setA(1 << 6);

	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, BitZeroPageSetsNegativeFlag) {
	
	Byte ins = cpu.Opcodes.BIT_ZP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x01;
	mem[0x0001] = 0x8F;
	cpu.setA(1 << 7);

	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}


