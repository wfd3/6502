#include <gtest/gtest.h>
#include <6502.h>

class MOS6502PushPopTests : public testing::Test {
public:

	Memory<Address_t, Byte> mem{CPU::MAX_MEM};
	CPU cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, CPU::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}
};

TEST_F(MOS6502PushPopTests, PhaImmediate) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_PHA_IMP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.A = 0x52;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_EQ(mem[0X01FF], 0x52);
	EXPECT_EQ(cpu.SP, CPU::INITIAL_SP - 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}


TEST_F(MOS6502PushPopTests, PlaImmediate) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_PLA_IMP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR, CPU::INITIAL_SP - 1);
	
	mem[0xFFFC] = ins;
	mem[0x01FF] = 0x52;
	cpu.A = 0xFF;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, 0x52);
	EXPECT_EQ(cpu.SP, CPU::INITIAL_SP);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502PushPopTests, PhpImmediate) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_PHP_IMP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0x01FF] = 0x52;
	cpu.PS = 0b01010101;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_EQ(mem[0x01FF], 0b01110101);
	EXPECT_EQ(cpu.SP, CPU::INITIAL_SP - 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502PushPopTests, PlpImmediate) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_PLP_IMP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR, CPU::INITIAL_SP - 1);
	

	mem[0xFFFC] = ins;
	mem[0x01FF] = 0b01010101;;
	cpu.PS = 0x52;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_EQ(cpu.PS, 0b01000101);
	EXPECT_EQ(cpu.SP, CPU::INITIAL_SP);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
