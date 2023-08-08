#include <gtest/gtest.h>
#include "../6502.h"

class MOS6502XXXFunctionalTestSuite : public testing::Test {
public:	
	Memory mem{CPU::MAX_MEM};
	CPU cpu{&mem};

	virtual void SetUp() {
		cpu.Reset(CPU::INITIAL_PC);
		mem.Init();
	}

	virtual void TearDown() {
	}
};

// This test never exits
TEST_F(MOS6502XXXFunctionalTestSuite, TestLoad6502TestSuite)
{
#if 1
	// Given:
	cpu.Reset(0x400);

	// When:
	mem.LoadProgramFromFile("./tests/6502_functional_test.bin", 0x0000);

	//Then:
	cpu.Debug();
#endif
}


