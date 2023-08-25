#include <gtest/gtest.h>
#include "../6502.h"

class MOS6502PushPopTests : public testing::Test {
public:

	Memory mem{CPU::MAX_MEM};
	CPU cpu{&mem};

	virtual void SetUp() {
		cpu.exitReset();
		mem.mapRAM(0, CPU::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}
};

TEST_F(MOS6502PushPopTests, PhaImmediate) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_PHA_IMP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	cpu.A = 0x52;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(mem[0X01FF], 0x52);
	EXPECT_EQ(cpu.SP, CPU::INITIAL_SP - 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}


TEST_F(MOS6502PushPopTests, PlaImmediate) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_PLA_IMP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0x01FF] = 0x52;
	cpu.SP = CPU::INITIAL_SP - 1;
	cpu.A = 0xFF;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, 0x52);
	EXPECT_EQ(cpu.SP, CPU::INITIAL_SP);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502PushPopTests, PhpImmediate) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_PHP_IMP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0x01FF] = 0x52;
	cpu.SP = CPU::INITIAL_SP;
	cpu.PS = 0b01010101;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(mem[0x01FF], 0b01110101);
	EXPECT_EQ(cpu.SP, CPU::INITIAL_SP - 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502PushPopTests, PlpImmediate) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_PLP_IMP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0x01FF] = 0b01010101;;
	cpu.SP = CPU::INITIAL_SP - 1;
	cpu.PS = 0x52;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.PS, 0b01000101);
	EXPECT_EQ(cpu.SP, CPU::INITIAL_SP);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
