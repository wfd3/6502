#include <gtest/gtest.h>
#include "../6502.h"

class MOS6502RORROLTests : public testing::Test {
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

TEST_F(MOS6502RORROLTests, RolAccumulator) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ROL_ACC;
	Byte data = 0b01010101;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	cpu.A = data;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(cpu.A, data << 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502RORROLTests, RolAccumulatorSetsCarryFlag) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ROL_ACC;
	Byte data = 0b10000001;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	cpu.A = data;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.A, Byte(data << 1));
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502RORROLTests, RolZeroPage) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ROL_ZP;
	Byte data = 0b01010101;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0x0020] = data;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(mem[0x0020], data << 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502RORROLTests, RolZeroPageX) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ROL_ZPX;
	Byte data = 0b01010101;
	
	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	cpu.X = 0x10;
	mem[0x0020] = data;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(mem[0x0020], data << 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502RORROLTests, RolAbsoltute) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ROL_ABS;
	Byte data = 0b01010101;
	
	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = data;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(mem[0x2000], data << 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502RORROLTests, RolAbsoltuteX) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ROL_ABX;
	Byte data = 0b01010101;
	
	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.X = 0x05;
	mem[0x2005] = data;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(mem[0x2005], Byte(data << 1));
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// ROR
TEST_F(MOS6502RORROLTests, RorAccumulator) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ROR_ACC;
	Byte data = 0b01010101;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	cpu.A = data;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.A, data >> 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502RORROLTests, RorAccumulatorSetsCarryFlag) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ROR_ACC;
	Byte data = 0b10000001;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	cpu.A = data;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.A, Byte(data >> 1));
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502RORROLTests, RorAccumulatorClearsCarryAndSetsNegativeFlag) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ROR_ACC;
	Byte data = 0b10000000;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	cpu.A = data;
	cpu.Flags.C = 1;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(cpu.A, Byte(data >> 1) | (1 << 7));
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502RORROLTests, RorZeroPage) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ROR_ZP;
	Byte data = 0b01010101;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0x0020] = data;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(mem[0x0020], data >> 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502RORROLTests, RorZeroPageX) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ROR_ZPX;
	Byte data = 0b01010101;
	
	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	cpu.X = 0x10;
	mem[0x0020] = data;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(mem[0x0020], data >> 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

//HERE
TEST_F(MOS6502RORROLTests, RorAbsoltute) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ROR_ABS;
	Byte data = 0b01010101;
	
	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = data;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(mem[0x2000], data >> 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502RORROLTests, RorAbsoltuteX) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ROR_ABX;
	Byte data = 0b01010101;
	
	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.X = 0x05;
	mem[0x2005] = data;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(mem[0x2005], Byte(data >> 1));
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
