//
// Tests lda, ldx and ldy instruction handling for 6502 
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

void testClass::TestLD(Byte ins, Registers r) {
	
	Byte reg;
	static constexpr Byte VAL = 0xff;

	//Given:
	mem[0xFFFC] = ins;
	setReg(r, VAL);
	
	//When:
	cpu.execute();

	// Then:
	reg = getReg(r);
	EXPECT_EQ(reg, 0x0F);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

// Test STA, STX and STY
void testClass::TestST(Byte ins, Word addr, Registers r) {
	
	Byte reg = 0;

	//Given:
	mem[0xFFFC] = ins;

	// When:
	cpu.execute();

	//Then:
	reg = getReg(r);
	EXPECT_EQ(mem[addr], reg);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}
	 
// LDA
TEST_F(testClass, LDAImmediate) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	

	mem[0xFFFD] = 0x0F;
	TestLD(cpu.Opcodes.LDA_IMM, Registers::A);
}

TEST_F(testClass, LDAZeroPage) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	

	mem[0xFFFD] = 0x0F;
	mem[0x000F] = 0x0F;
	TestLD(cpu.Opcodes.LDA_ZP, Registers::A);
}

TEST_F(testClass, LDAZeroPageX) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	

	mem[0xFFFD] = 0x0F;
	mem[0x000F] = 0x0F;
	cpu.setX(0);
	TestLD(cpu.Opcodes.LDA_ZPX, Registers::A);
}

TEST_F(testClass, LDAAbsolute) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x0F;
	TestLD(cpu.Opcodes.LDA_ABS, Registers::A);
}

TEST_F(testClass, LDAAbsoluteX) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setX(0);
	mem[0x2000] = 0x0F;
	TestLD(cpu.Opcodes.LDA_ABX, Registers::A);
}

// LDX
TEST_F(testClass, LDXImmediate) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFD] = 0x0F;
	TestLD(cpu.Opcodes.LDX_IMM, Registers::X);
}

TEST_F(testClass, LDXZeroPage) {
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFD] = 0x0F;
	mem[0x000F] = 0x0F;
	TestLD(cpu.Opcodes.LDX_ZP, Registers::X);
}


TEST_F(testClass, LDXZeroPageY) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFD] = 0x0F;
	mem[0x000F] = 0x0F;
	cpu.setY(0);
	TestLD(cpu.Opcodes.LDX_ZPY, Registers::X);
}

TEST_F(testClass, LDXAbsolute) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x0F;
	TestLD(cpu.Opcodes.LDX_ABS, Registers::X);
}

TEST_F(testClass, LDXAbsoluteX) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setY(0);
	mem[0x2000] = 0x0F;
	TestLD(cpu.Opcodes.LDY_ABX, Registers::Y);
}

// LDY
TEST_F(testClass, LDYImmediate) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFD] = 0x0F;
	TestLD(cpu.Opcodes.LDY_IMM, Registers::Y);
}

TEST_F(testClass, LDYZeroPage) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFD] = 0x0F;
	mem[0x000F] = 0x0F;
	TestLD(cpu.Opcodes.LDY_ZP, Registers::Y);
}

TEST_F(testClass, LDYZeroPageX) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFD] = 0x0F;
	mem[0x000F] = 0x0F;
	cpu.setX(0);
	TestLD(cpu.Opcodes.LDY_ZPX, Registers::Y);
}

TEST_F(testClass, LDYAbsolute) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
		
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x0F;
	TestLD(cpu.Opcodes.LDY_ABS, Registers::Y);
}

TEST_F(testClass, LDYAbsoluteX) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setX(0);
	mem[0x2000] = 0x0F;
	TestLD(cpu.Opcodes.LDY_ABX, Registers::Y);
}

// STA
TEST_F(testClass, STAAbsolute) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setA(0x52);
	TestST(cpu.Opcodes.STA_ABS, 0x2000, Registers::A);
}

TEST_F(testClass, STAZeroPage) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFD] = 0x20;
	cpu.setA(0x52);
	TestST(cpu.Opcodes.STA_ZP, 0x0020, Registers::A);
}

TEST_F(testClass, STAZeroPageX) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFD] = 0x20;
	cpu.setX(0x02);
	cpu.setA(0x52);
	TestST(cpu.Opcodes.STA_ZPX, 0x0022, Registers::A);
}

TEST_F(testClass, STAAbsoluteX) {
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setX(0x02);
	cpu.setA(0x52);
	TestST(cpu.Opcodes.STA_ABX, 0x2002, Registers::A);
}

TEST_F(testClass, STAAbsoluteY) {
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setY(0x02);
	cpu.setA(0x52);
	TestST(cpu.Opcodes.STA_ABY, 0x2002, Registers::A);
}

TEST_F(testClass, STAIndirectX) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFD] = 0x10;
	cpu.setX(0x10);
	mem[0x0020] = 0x00;
	mem[0x0021] = 0x20;
	cpu.setA(0x52);
	TestST(cpu.Opcodes.STA_IDX, 0x2000, Registers::A);
}

TEST_F(testClass, STAIndirectY) {
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFD] = 0x00;
	mem[0x0000] = 0x00;
	mem[0x0001] = 0x20;
	cpu.setY(0x02);
	cpu.setA(0x52);
	TestST(cpu.Opcodes.STA_IDY, 0x2002, Registers::A);
}

// STX
TEST_F(testClass, STXZeroPage) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFD] = 0x20;
	cpu.setX(0x52);
	TestST(cpu.Opcodes.STX_ZP, 0x0020, Registers::X);
}

TEST_F(testClass, STXZeroPageY) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFD] = 0x20;
	cpu.setY(0x02);
	cpu.setX(0x52);
	TestST(cpu.Opcodes.STX_ZPY, 0x0022, Registers::X);
}

TEST_F(testClass, STXAbsolute) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setX(0x52);
	TestST(cpu.Opcodes.STX_ABS, 0x2000, Registers::X);
}

// STY
TEST_F(testClass, STYZeroPage) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFD] = 0x20;
	cpu.setY(0x52);
	TestST(cpu.Opcodes.STY_ZP, 0x0020, Registers::Y);
}

TEST_F(testClass, STYZeroPageX) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFD] = 0x20;
	cpu.setX(0x02);
	cpu.setY(0x52);
	TestST(cpu.Opcodes.STY_ZPX, 0x0022, Registers::Y);
}

TEST_F(testClass, STYAbsolute) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setY(0x52);
	TestST(cpu.Opcodes.STY_ABS, 0x2000, Registers::Y);
}
