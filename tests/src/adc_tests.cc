//
// Tests for adc and sbc instructions for 6502 only
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

TEST_F(testClass, ADCImmediateAddsPositiveNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.ADC_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	cpu.setA(0x10);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0x20);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, ADCZeroPageAddsPositiveNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.ADC_ZP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	mem[0x0010] = 0x10;
	cpu.setA(0x10);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0x20);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, ADCZeroPageXAddsPositiveNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.ADC_ZPX;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	cpu.setX(0x10);
	mem[0x0020] = 0x10;
	cpu.setA(0x10);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0x20);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, ADCAbsoluteAddsPositiveNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.ADC_ABS;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0xFFFE] = 0x00;
	mem[0x0020] = 0x10;
	cpu.setA(0x10);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0x20);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, ADCAbsoluteXAddsPositiveNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.ADC_ABX;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0xFFFE] = 0x00;
	cpu.setX(0x01);
	mem[0x0021] = 0x10;
	cpu.setA(0x10);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0x20);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, ADCAbsoluteXCrossPageBoundaryAddsPositiveNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.ADC_ABX;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xfe;
	mem[0xFFFE] = 0x00;
	cpu.setX(0x0f);
	mem[0x010d] = 0x10;
	cpu.setA(0x10);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0x20);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, ADCAbsoluteYAddsPositiveNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.ADC_ABY;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0xFFFE] = 0x00;
	cpu.setY(0x01);
	mem[0x0021] = 0x10;
	cpu.setA(0x10);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0x20);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_EQ(UsedCycles, ExpectedCycles);  
}

TEST_F(testClass, ADCAbsoluteYCrossPageBoundaryAddsPositiveNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.ADC_ABY;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xfe;
	mem[0xFFFE] = 0x00;
	cpu.setY(0x0f);
	mem[0x010d] = 0x10;
	cpu.setA(0x10);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0x20);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, ADCIndirectXAddsPositiveNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.ADC_IDX;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	cpu.setX(0x20);
	mem[0x0040] = 0x00;
	mem[0x0041] = 0x20;
	mem[0x2000] = 0x10;
	cpu.setA(0x10);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0x20);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, ADCIndirectYAddsPositiveNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.ADC_IDY;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	cpu.setY(0x20);
	mem[0x0020] = 0x00;
	mem[0x0021] = 0x20;
	mem[0x2020] = 0x10;
	cpu.setA(0x10);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0x20);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, ADCIndirectYCrossPageAddsPositiveNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.ADC_IDY;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	cpu.setY(0x0f);
	mem[0x0020] = 0xfe;
	mem[0x0021] = 0x00;
	mem[0x010d] = 0x10;
	cpu.setA(0x10);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0x20);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, ADCImmediateAddsWithCarry) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.ADC_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	cpu.setA(0x10);
	cpu.setFlagC(true);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0x21);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, ADCImmediateAddsNegativeNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.ADC_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = Byte(-1);
	cpu.setA(Byte(-2));
	cpu.setFlagC(false);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), Byte(-3));
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, ADCImmediateAddsNegativeNumbersWithCarry) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.ADC_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = Byte(-1);
	cpu.setA(Byte(-2));
	cpu.setFlagC(true);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), Byte(-2));
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, ADCImmediateAddsZero) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.ADC_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	cpu.setA(0);
	cpu.setFlagC(false);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0);
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, ADCImmediateAddsWithCarryOverflow) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.ADC_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xf0;
	cpu.setA(0x20);
	cpu.setFlagC(false);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), Byte(0xf0+0x20));
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, ADCImmediateAddsWithOverflow) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.ADC_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = Byte(-128);
	cpu.setA(Byte(-1));
	cpu.setFlagC(false);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 127);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// SBC tests
TEST_F(testClass, SBCImmediateSubtractsPositiveNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.SBC_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x01;
	cpu.setA(0x09);
	cpu.setFlagC(true);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0x09 - 0x01);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, SBCImmediateSubtractsZeroNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.SBC_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x0;
	cpu.setA(0x0);
	cpu.setFlagC(true);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0x0);
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 

}
TEST_F(testClass, SBCImmediateSubtractsZeroWitCarryNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.SBC_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x0;
	cpu.setA(0);
	cpu.setFlagC(false);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), Byte(-1));
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}


TEST_F(testClass, SBCImmediateSubtractsPosAndNegWithSignedOverflow) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.SBC_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = Byte(-1);
	cpu.setA(127);
	cpu.setFlagC(true);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 128);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagV());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// BCD ADC tests
TEST_F(testClass, ADCImmediateAddsPositiveBCDNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.ADC_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x28;
	cpu.setA(0x32);
	cpu.setFlagD(true);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0x60);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, ADCImmediateAddsPositiveBCDNumbersWithCarry) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.ADC_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x28;
	cpu.setA(0x32);
	cpu.setFlagD(true);
	cpu.setFlagC(true);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:n
	EXPECT_EQ(cpu.getA(), 0x60 + 1);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, ADCImmediateAddsZeroBCDNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.ADC_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x0;
	cpu.setA(0);
	cpu.setFlagD(true);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0);
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, ADCImmediateAddsLargePositiveBCDNumbersWithCarry) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.ADC_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	// 99 + 99 + 1 = 98, C=1, N=1, V=1
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x99;
	cpu.setA(0x99);
	cpu.setFlagD(true);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0x98);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagV());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// BCD SBC

TEST_F(testClass, SBCImmediateSubtractsPositiveBCDNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.SBC_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x21;
	cpu.setA(0x51);
	cpu.setFlagD(true);
	cpu.setFlagC(true);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0x30);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, SBCImmediateSubtractsPositiveBCDNumbersAndGetsZero) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.SBC_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x51;
	cpu.setA(0x51);
	cpu.setFlagD(true);
	cpu.setFlagC(true);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0);
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, SBCImmediateSubtractsPositiveBCDNumbersAndGetsZeroWhenCarryNotSet) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.SBC_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x1;
	cpu.setA(0x2);
	cpu.setFlagD(true);
	cpu.setFlagC(false);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0);
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, SBCImmediateSubtractsPositiveBCDNumbersAndGetsNegativeWhenCarrySet) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.SBC_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x34;
	cpu.setA(0x21);
	cpu.setFlagD(true);
	cpu.setFlagC(true);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0x87);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, SBCImmediateSubtractsSimple) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.SBC_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x01;
	cpu.setA(0);
	cpu.setFlagD(true);
	cpu.setFlagC(true);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0x99);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(testClass, ADCBCDOnePlus99EqualsZero) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.ADC_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x99;
	cpu.setA(0x01);
	cpu.setFlagD(true);
	cpu.setFlagC(false);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0x00);
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
