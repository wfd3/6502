#include <gtest/gtest.h>
#include "../6502.h"

class MOS6502XXXFunctionalTestSuite : public testing::Test {
public:	
	Memory mem{CPU::MAX_MEM};
	CPU cpu{&mem};

	virtual void SetUp() {
		cpu.exitReset();
		mem.Init();
	}

	virtual void TearDown() {
	}
};

// This test takes time to run.
// If it completes, it passed.  If it drops into the debugger with a
// Loop Detected notice, it's failed.
TEST_F(MOS6502XXXFunctionalTestSuite, TestLoad6502TestSuite)
{
#if 1
	// Given:
	constexpr Word exitAddress = 0x3469;
	// When:
	mem.LoadProgramFromFile("./tests/6502_functional_test.bin", 0x0000);
	cpu.setResetVector(0x0400);
	cpu.exitReset();
	cpu.setExitAddress(exitAddress);
	cpu.toggleLoopDetection(); // Force break on 'jmp *'

	//Then:
	printf("This test takes some time...\n");

// Uncomment to start in debugger
//	cpu.SetDebug(true);
	cpu.Execute();

	EXPECT_EQ(cpu.PC, exitAddress);
#endif
}


