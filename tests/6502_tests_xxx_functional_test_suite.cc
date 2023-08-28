#include <gtest/gtest.h>
#include "../6502.h"

class MOS6502XXXFunctionalTestSuite : public testing::Test {
public:	
	Memory<Address_t, Byte> mem{CPU::MAX_MEM};
	CPU cpu{&mem};

	virtual void SetUp() {
		cpu.exitReset();
		mem.mapRAM(0, CPU::MAX_MEM);
	}

	virtual void TearDown() {
	}
};

// This test takes time to run.
// If it completes, it passed.  If it drops into the debugger with a
// Loop Detected notice, it's failed.  To disable, comment out the next line
#define RUN_FUNCTIONAL_TEST

TEST_F(MOS6502XXXFunctionalTestSuite, TestLoad6502TestSuite)
{
#ifdef RUN_FUNCTIONAL_TEST
	// Given:
	constexpr Word exitAddress = 0x3469;

	// When:
	mem.loadDataFromFile("./binfiles/6502_functional_test.bin", 0x0000);
	cpu.setResetVector(0x0400);
	cpu.exitReset();
	cpu.setExitAddress(exitAddress);
	cpu.toggleLoopDetection(); // Force break on 'jmp *'
	cpu.Cycles.disableTimingEmulation();

	//Then:
	printf("# This test takes some time...\n");

// Uncomment to start in debugger
//	cpu.SetDebug(true);
	cpu.Execute();

	EXPECT_EQ(cpu.PC, exitAddress);
#endif
}


