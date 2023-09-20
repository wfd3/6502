#include <gtest/gtest.h>
#include <6502.h>

class MOS6502JMPTests : public testing::Test {
public:

	Memory<Address_t, Byte> mem{CPU::MAX_MEM};
	CPU cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, CPU::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}
};

TEST_F(MOS6502JMPTests, JmpAbsolute) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_JMP_ABS;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x21;
	mem[0xFFFE] = 0x43;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_EQ(cpu.PC, 0x4321);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502JMPTests, JmpIndirect) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_JMP_IND;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x21;
	mem[0x2001] = 0x43;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_EQ(cpu.PC, 0x4321);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
