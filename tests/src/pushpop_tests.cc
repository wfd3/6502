//
// Tests the push/pop instructions
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

// These tests have to know the starting value of the SP.  Set it here
constexpr Byte START_SP_ADDRESS = 0xff;

TEST_F(testClass, PhaImmediate) {
	
	Byte ins = cpu.Opcodes.PHA_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(0x52);

	//When:
	cpu.execute();

	// Then:
	EXPECT_EQ(mem[0X01FF], 0x52);
	EXPECT_EQ(cpu.getSP(), START_SP_ADDRESS - 1);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, PlaImmediate) {
	
	Byte ins = cpu.Opcodes.PLA_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR, START_SP_ADDRESS - 1);
	
	mem[0xFFFC] = ins;
	mem[0x01FF] = 0x52;
	cpu.setA(0xff);

	//When:
	cpu.execute();

	// Then:
	EXPECT_EQ(cpu.getA(), 0x52);
	EXPECT_EQ(cpu.getSP(), START_SP_ADDRESS);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, PhpImmediate) {
	
	Byte ins = cpu.Opcodes.PHP_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0x01FF] = 0x52;
	cpu.setPS(0b01010101);

	//When:
	cpu.execute();

	// Then:
	EXPECT_EQ(mem[0x01FF], 0b01110101);
	EXPECT_EQ(cpu.getSP(), START_SP_ADDRESS - 1);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, PlpImmediate) {
	
	Byte ins = cpu.Opcodes.PLP_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR, START_SP_ADDRESS - 1);

	mem[0xFFFC] = ins;
	mem[0x01FF] = 0b01010101;;
	cpu.setPS(0x52);

	//When:
	cpu.execute();

	// Then:
	EXPECT_EQ(cpu.getPS(), 0b01000101);
	EXPECT_EQ(cpu.getSP(), START_SP_ADDRESS);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}
