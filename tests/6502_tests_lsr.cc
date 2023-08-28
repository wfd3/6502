#include <gtest/gtest.h>
#include "../6502.h"

class MOS6502LSRTests : public testing::Test {
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

TEST_F(MOS6502LSRTests, LsrAccumulator) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_LSR_ACC;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	cpu.A = 0b01010101;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.A, 0b00101010);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502LSRTests, LsrZeroPage) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_LSR_ZP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0x0020] = 0b01010101;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(mem[0x0020], 0b00101010);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502LSRTests, LsrZeroPageX) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_LSR_ZPX;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	cpu.X = 0x10;
	mem[0x0020] = 0b01010101;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(mem[0x0020], 0b00101010);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502LSRTests, LsrAbsoltute) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_LSR_ABS;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0b01010101;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(mem[0x2000], 0b00101010);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502LSRTests, LsrAbsoltuteX) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_LSR_ABX;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.X = 0x05;
	mem[0x2005] = 0b01010101;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(mem[0x2005], 0b00101010);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

