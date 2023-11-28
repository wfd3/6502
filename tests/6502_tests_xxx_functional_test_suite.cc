#include <gtest/gtest.h>
#include <6502.h>

// This test takes time to run.
// If it completes, it passed.  If it drops into the debugger with a
// Loop Detected notice, it's failed.  To disable, comment out the next line
#define RUN_FUNCTIONAL_TEST

#ifdef RUN_FUNCTIONAL_TEST
static const char *fileName = BINFILE_PATH "/6502_functional_test.bin";
#endif

class MOS6502XXXFunctionalTestSuite : public testing::Test {
public:	
	Memory<Address_t, Byte> mem{CPU::MAX_MEM};
	CPU cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, CPU::MAX_MEM);
	}

	virtual void TearDown() {
	}
};


TEST_F(MOS6502XXXFunctionalTestSuite, TestLoad6502TestSuite)
{
#ifdef RUN_FUNCTIONAL_TEST
	// Given:
	constexpr Word exitAddress = 0x3469;

	// When:
	mem.loadDataFromFile(fileName, 0x0000);
	cpu.setResetVector(0x0400);
	cpu.setExitAddress(exitAddress);
	cpu.toggleLoopDetection(); // Force break on 'jmp *'
	cpu.Reset();

	//Then:
	std::cout << "# This test takes about 20 seconds..." << std::endl;

	// Uncomment to start in debugger
	//cpu.SetDebug(true);
	
	while (!cpu.executeOne())
		;

	EXPECT_EQ(cpu.PC, exitAddress);
#endif
}


