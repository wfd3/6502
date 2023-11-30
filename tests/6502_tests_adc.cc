#include <gtest/gtest.h>
#include <6502.h>

class MOS6502ADCTests : public testing::Test {
public:
	Memory<Address_t, Byte> mem{CPU::MAX_MEM};
	CPU cpu{mem};


	virtual void SetUp() {
		mem.mapRAM(0, CPU::MAX_MEM);
	}

	virtual void TearDown()	{
	}
};

TEST_F(MOS6502ADCTests, ADCImmediateAddsPositiveNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ADC_IMM;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	cpu.A = 0x10;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.A, 0x20);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCZeroPageAddsPositiveNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ADC_ZP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	mem[0x0010] = 0x10;
	cpu.A = 0x10;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.A, 0x20);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCZeroPageXAddsPositiveNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ADC_ZPX;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	cpu.X = 0x10;
	mem[0x0020] = 0x10;
	cpu.A = 0x10;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.A, 0x20);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCAbsoluteAddsPositiveNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ADC_ABS;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0xFFFE] = 0x00;
	mem[0x0020] = 0x10;
	cpu.A = 0x10;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.A, 0x20);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCAbsoluteXAddsPositiveNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ADC_ABX;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0xFFFE] = 0x00;
	cpu.X = 0x01;
	mem[0x0021] = 0x10;
	cpu.A = 0x10;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.A, 0x20);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCAbsoluteXCrossPageBoundaryAddsPositiveNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ADC_ABX;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xfe;
	mem[0xFFFE] = 0x00;
	cpu.X = 0x0f;
	mem[0x010d] = 0x10;
	cpu.A = 0x10;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.A, 0x20);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCAbsoluteYAddsPositiveNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ADC_ABY;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0xFFFE] = 0x00;
	cpu.Y = 0x01;
	mem[0x0021] = 0x10;
	cpu.A = 0x10;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.A, 0x20);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCAbsoluteYCrossPageBoundaryAddsPositiveNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ADC_ABY;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xfe;
	mem[0xFFFE] = 0x00;
	cpu.Y = 0x0f;
	mem[0x010d] = 0x10;
	cpu.A = 0x10;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.A, 0x20);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCIndirectXAddsPositiveNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ADC_IDX;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	cpu.X = 0x20;
	mem[0x0040] = 0x00;
	mem[0x0041] = 0x20;
	mem[0x2000] = 0x10;
	cpu.A = 0x10;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.A, 0x20);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCIndirectYAddsPositiveNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ADC_IDY;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	cpu.Y = 0x20;
	mem[0x0020] = 0x00;
	mem[0x0021] = 0x20;
	mem[0x2020] = 0x10;
	cpu.A = 0x10;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.A, 0x20);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCIndirectYCrossPageAddsPositiveNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ADC_IDY;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	cpu.Y = 0x0f;
	mem[0x0020] = 0xfe;
	mem[0x0021] = 0x00;
	mem[0x010d] = 0x10;
	cpu.A = 0x10;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.A, 0x20);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCImmediateAddsWithCarry) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ADC_IMM;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	cpu.A = 0x10;
	cpu.Flags.C = 1;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.A, 0x21);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCImmediateAddsNegativeNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ADC_IMM;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = Byte(-1);
	cpu.A = Byte(-2);
	cpu.Flags.C = 0;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.A, Byte(-3));
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCImmediateAddsNegativeNumbersWithCarry) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ADC_IMM;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = Byte(-1);
	cpu.A = Byte(-2);
	cpu.Flags.C = 1;;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.A, Byte(-2));
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCImmediateAddsZero) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ADC_IMM;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	cpu.A = 0x00;
	cpu.Flags.C = 0;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.A, 0);
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCImmediateAddsWithCarryOverflow) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ADC_IMM;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xf0;
	cpu.A = 0x20;
	cpu.Flags.C = 0;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.A, Byte(0xf0+0x20));
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCImmediateAddsWithOverflow) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ADC_IMM;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = Byte(-128);
	cpu.A = Byte(-1);
	cpu.Flags.C = 0;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.A, 127);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// SBC tests
TEST_F(MOS6502ADCTests, SBCImmediateSubtractsPositiveNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_SBC_IMM;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x01;
	cpu.A = 0x09;
	cpu.Flags.C = 1;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.A, 0x09 - 0x01);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, SBCImmediateSubtractsZeroNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_SBC_IMM;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x0;
	cpu.A = 0x0;
	cpu.Flags.C = 1;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.A, 0x0);
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 

}
TEST_F(MOS6502ADCTests, SBCImmediateSubtractsZeroWitCarryNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_SBC_IMM;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x0;
	cpu.A = 0x0;
	cpu.Flags.C = 0;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.A, Byte(-1));
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}


TEST_F(MOS6502ADCTests, SBCImmediateSubtractsPosAndNegWithSignedOverflow) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_SBC_IMM;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = Byte(-1);
	cpu.A = 127;
	cpu.Flags.C = 1;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.A, 128);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.V);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// BCD ADC tests
TEST_F(MOS6502ADCTests, ADCImmediateAddsPositiveBCDNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ADC_IMM;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x28;
	cpu.A = 0x32;
	cpu.Flags.D = 1;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.A, 0x60);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCImmediateAddsPositiveBCDNumbersWithCarry) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ADC_IMM;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x28;
	cpu.A = 0x32;
	cpu.Flags.D = 1;
	cpu.Flags.C = 1;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:n
	EXPECT_EQ(cpu.A, 0x60 + 1);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCImmediateAddsZeroBCDNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ADC_IMM;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x0;
	cpu.A = 0x0;
	cpu.Flags.D = 1;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.A, 0);
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCImmediateAddsLargePositiveBCDNumbersWithCarry) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ADC_IMM;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	

	// 99 + 99 + 1 = 98, C=1, N=1, V=1
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x99;
	cpu.A = 0x99;
	cpu.Flags.D = 1;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.A, 0x98);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.V);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// BCD SBC

TEST_F(MOS6502ADCTests, SBCImmediateSubtractsPositiveBCDNumbers) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_SBC_IMM;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x21;
	cpu.A = 0x51;
	cpu.Flags.D = 1;
	cpu.Flags.C = 1;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.A, 0x30);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, SBCImmediateSubtractsPositiveBCDNumbersAndGetsZero) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_SBC_IMM;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x51;
	cpu.A = 0x51;
	cpu.Flags.D = 1;
	cpu.Flags.C = 1;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.A, 0);
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, SBCImmediateSubtractsPositiveBCDNumbersAndGetsZeroWhenCarryNotSet) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_SBC_IMM;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x1;
	cpu.A = 0x2;
	cpu.Flags.D = 1;
	cpu.Flags.C = 0;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.A, 0);
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, SBCImmediateSubtractsPositiveBCDNumbersAndGetsNegativeWhenCarrySet) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_SBC_IMM;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x34;
	cpu.A = 0x21;
	cpu.Flags.D = 1;
	cpu.Flags.C = 1;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.A, 0x87);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, SBCImmediateSubtractsSimple) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_SBC_IMM;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x01;
	cpu.A = 0x00;
	cpu.Flags.D = 1;
	cpu.Flags.C = 1;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.A, 0x99);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCBCDOnePlus99EqualsZero) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ADC_IMM;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x99;
	cpu.A = 0x01;
	cpu.Flags.D = 1;
	cpu.Flags.C = 0;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.A, 0x00);
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
