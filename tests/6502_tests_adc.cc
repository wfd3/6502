#include <gtest/gtest.h>
#include <6502.h>

class MOS6502ADCTests : public testing::Test {
public:
	Memory<Address_t, Byte> mem{CPU::MAX_MEM};
	CPU cpu{&mem};


	virtual void SetUp() {
		cpu.exitReset();
		mem.mapRAM(0, CPU::MAX_MEM);
	}

	virtual void TearDown()	{
	}
};

TEST_F(MOS6502ADCTests, ADCImmediateAddsPositiveNumbers) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ADC_IMM;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);

	printf("PC Post reset = %04x\n", cpu.PC);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	cpu.A = 0x10;
	printf("PC = %04x\n", cpu.PC);

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, 0x20);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCZeroPageAddsPositiveNumbers) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ADC_ZP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	mem[0x0010] = 0x10;
	cpu.A = 0x10;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, 0x20);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCZeroPageXAddsPositiveNumbers) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ADC_ZPX;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	cpu.X = 0x10;
	mem[0x0020] = 0x10;
	cpu.A = 0x10;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, 0x20);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCAbsoluteAddsPositiveNumbers) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ADC_ABS;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0xFFFE] = 0x00;
	mem[0x0020] = 0x10;
	cpu.A = 0x10;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, 0x20);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCAbsoluteXAddsPositiveNumbers) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ADC_ABX;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0xFFFE] = 0x00;
	cpu.X = 0x01;
	mem[0x0021] = 0x10;
	cpu.A = 0x10;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, 0x20);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCAbsoluteXCrossPageBoundryAddsPositiveNumbers) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ADC_ABX;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xfe;
	mem[0xFFFE] = 0x00;
	cpu.X = 0x0f;
	mem[0x010d] = 0x10;
	cpu.A = 0x10;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, 0x20);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCAbsoluteYAddsPositiveNumbers) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ADC_ABY;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0xFFFE] = 0x00;
	cpu.Y = 0x01;
	mem[0x0021] = 0x10;
	cpu.A = 0x10;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, 0x20);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCAbsoluteYCrossPageBoundryAddsPositiveNumbers) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ADC_ABY;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xfe;
	mem[0xFFFE] = 0x00;
	cpu.Y = 0x0f;
	mem[0x010d] = 0x10;
	cpu.A = 0x10;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, 0x20);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCIndirectXAddsPositiveNumbers) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ADC_IDX;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	cpu.X = 0x20;
	mem[0x0040] = 0x00;
	mem[0x0041] = 0x20;
	mem[0x2000] = 0x10;
	cpu.A = 0x10;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, 0x20);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCIndirectYAddsPositiveNumbers) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ADC_IDY;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	cpu.Y = 0x20;
	mem[0x0020] = 0x00;
	mem[0x0021] = 0x20;
	mem[0x2020] = 0x10;
	cpu.A = 0x10;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, 0x20);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCIndirectYCrossPageAddsPositiveNumbers) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ADC_IDY;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	cpu.Y = 0x0f;
	mem[0x0020] = 0xfe;
	mem[0x0021] = 0x00;
	mem[0x010d] = 0x10;
	cpu.A = 0x10;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, 0x20);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCImmediateAddsWithCarry) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ADC_IMM;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	cpu.A = 0x10;
	cpu.Flags.C = 1;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, 0x21);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCImmediateAddsNegativeNumbers) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ADC_IMM;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = Byte(-1);
	cpu.A = Byte(-2);
	cpu.Flags.C = 0;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, Byte(-3));
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCImmediateAddsNegativeNumbersWithCarry) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ADC_IMM;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = Byte(-1);
	cpu.A = Byte(-2);
	cpu.Flags.C = 1;;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, Byte(-2));
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCImmediateAddsZero) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ADC_IMM;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	cpu.A = 0x00;
	cpu.Flags.C = 0;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, 0);
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCImmediateAddsWithCarryOverflow) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ADC_IMM;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xf0;
	cpu.A = 0x20;
	cpu.Flags.C = 0;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, Byte(0xf0+0x20));
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCImmediateAddsWithOverflow) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ADC_IMM;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = Byte(-128);
	cpu.A = Byte(-1);
	cpu.Flags.C = 0;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

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
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_SBC_IMM;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x01;
	cpu.A = 0x09;
	cpu.Flags.C = 1;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, 0x09 - 0x01);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, SBCImmediateSubtractsZeroNumbers) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_SBC_IMM;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x0;
	cpu.A = 0x0;
	cpu.Flags.C = 1;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, 0x0);
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 

}
TEST_F(MOS6502ADCTests, SBCImmediateSubtractsZeroWitCarryNumbers) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_SBC_IMM;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x0;
	cpu.A = 0x0;
	cpu.Flags.C = 0;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, Byte(-1));
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}


TEST_F(MOS6502ADCTests, SBCImmediateSubtractsPosAndNegWithSignedOverflow) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_SBC_IMM;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = Byte(-1);
	cpu.A = 127;
	cpu.Flags.C = 1;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

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
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ADC_IMM;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x28;
	cpu.A = 0x32;
	cpu.Flags.D = 1;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, 0x60);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCImmediateAddsPositiveBCDNumbersWithCarry) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ADC_IMM;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x28;
	cpu.A = 0x32;
	cpu.Flags.D = 1;
	cpu.Flags.C = 1;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:n
	EXPECT_EQ(cpu.A, 0x60 + 1);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCImmediateAddsZeroBCDNumbers) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ADC_IMM;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x0;
	cpu.A = 0x0;
	cpu.Flags.D = 1;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, 0);
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCImmediateAddsLargePositiveBCDNumbersWithCarry) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ADC_IMM;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	// 99 + 99 + 1 = 98, C=1, N=1, V=1
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x99;
	cpu.A = 0x99;
	cpu.Flags.D = 1;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

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
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_SBC_IMM;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x21;
	cpu.A = 0x51;
	cpu.Flags.D = 1;
	cpu.Flags.C = 1;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, 0x30);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, SBCImmediateSubtractsPositiveBCDNumbersAndGetsZero) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_SBC_IMM;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x51;
	cpu.A = 0x51;
	cpu.Flags.D = 1;
	cpu.Flags.C = 1;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, 0);
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, SBCImmediateSubtractsPositiveBCDNumbersAndGetsZeroWhenCarryNotSet) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_SBC_IMM;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x1;
	cpu.A = 0x2;
	cpu.Flags.D = 1;
	cpu.Flags.C = 0;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, 0);
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, SBCImmediateSubtractsPositiveBCDNumbersAndGetsNegativeWhenCarrySet) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_SBC_IMM;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x34;
	cpu.A = 0x21;
	cpu.Flags.D = 1;
	cpu.Flags.C = 1;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, 0x87);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, SBCImmediateSubtractsSimple) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_SBC_IMM;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x01;
	cpu.A = 0x00;
	cpu.Flags.D = 1;
	cpu.Flags.C = 1;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, 0x99);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ADCTests, ADCBCDOnePlus99EqualsZero) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ADC_IMM;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x99;
	cpu.A = 0x01;
	cpu.Flags.D = 1;
	cpu.Flags.C = 0;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, 0x00);
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.V);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

