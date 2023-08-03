#include <gtest/gtest.h>
#include "../6502.h"

class MOS6502JSRTests : public testing::Test {
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

TEST_F(MOS6502JSRTests, JsrAbsolute) {
	CPU::Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_JSR_ABS;

	//Given:
	cpu.Reset(CPU::INITIAL_PC);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x21;
	mem[0xFFFE] = 0x43;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.PC, 0x4321);
	EXPECT_EQ(mem[0x01FF], 0xFC);
	EXPECT_EQ(mem[0x01FE], 0xFF);	
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
