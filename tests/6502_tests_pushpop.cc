#include <gtest/gtest.h>
#include "../6502.h"

class MOS6502PushPopTests : public testing::Test {
public:

	Memory mem{CPU::MAX_MEM};
	CPU cpu{&mem};

	virtual void SetUp() {
		cpu.Reset(CPU::INITIAL_PC);
		mem.Init();
	}
	
	virtual void TearDown()	{
	}
};

TEST_F(MOS6502PushPopTests, PhaImmediate) {
	CPU::Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_PHA_IMP;

	//Given:
	cpu.Reset(CPU::INITIAL_PC);
	mem.Init();

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
	CPU::Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_PLA_IMP;

	//Given:
	cpu.Reset(CPU::INITIAL_PC);
	mem.Init();

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
	CPU::Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_PHP_IMP;

	//Given:
	cpu.Reset(CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0x01FF] = 0x52;
	cpu.SP = CPU::INITIAL_SP;
	cpu.PS = 0b01010101;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(mem[0x01FF], 0b01010101);
	EXPECT_EQ(cpu.SP, CPU::INITIAL_SP - 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502PushPopTests, PlpImmediate) {
	CPU::Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_PLP_IMP;

	//Given:
	cpu.Reset(CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0x01FF] = 0b01010101;;
	cpu.SP = CPU::INITIAL_SP - 1;
	cpu.PS = 0x52;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.PS, 0b01010101);
	EXPECT_EQ(cpu.SP, CPU::INITIAL_SP);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
