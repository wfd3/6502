#include <gtest/gtest.h>
#include "../6502.h"

class MOS6502ASLTests : public testing::Test {
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

TEST_F(MOS6502ASLTests, AslAccumulator) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ASL_ACC;
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
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_EQ(cpu.A, data << 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ASLTests, AslAccumulatorSetsCarryFlag) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ASL_ACC;
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
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_EQ(cpu.A, Byte(data << 1));
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ASLTests, AslAccumulatorSetsNegativeFlag) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ASL_ACC;
	Byte data = 0b01000001;

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
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_EQ(cpu.A, Byte(data << 1));
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ASLTests, AslAccumulatorSetsZeroFlag) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ASL_ACC;
	Byte data = 0b00000000;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	cpu.A = data;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_EQ(cpu.A, Byte(data << 1));
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ASLTests, AslZeroPage) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ASL_ZP;
	Byte data = 0b01010101;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x0001;
	mem[0x0001] = data;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_EQ(mem[0x0001], data << 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ASLTests, AslZeroPageX) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ASL_ZPX;
	Byte data = 0b01010101;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x0001;
	cpu.X = 0x02;
	mem[0x0003] = data;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_EQ(mem[0x0003], data << 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ASLTests, AslAbsolute) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ASL_ABX;
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
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_EQ(mem[0x2000], data << 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ASLTests, AslAbsoluteX) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_ASL_ABX;
	Byte data = 0b01010101;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.X = 0x02;
	mem[0x2002] = data;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_EQ(mem[0x2002], data << 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}


