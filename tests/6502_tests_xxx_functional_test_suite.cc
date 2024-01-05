//
// Runs the 6502 functional tests from
//   https://github.com/Klaus2m5/6502_65C02_functional_tests
//
// Copyright (C) 2023 Walt Drummond
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.

#include <gtest/gtest.h>
#include <6502.h>

// This test takes time to run.
// If the test completes, it passed.  If it drops into the debugger with a
// Loop Detected notice, it's failed.  
// 
// To disable this tests comment out the next line
#define RUN_FUNCTIONAL_TEST

#ifdef RUN_FUNCTIONAL_TEST
static const char *fileName = BINFILE_PATH "/6502_functional_test.bin";
#endif

class MOS6502XXXFunctionalTestSuite : public testing::Test {
public:	
	Memory<Address_t, Byte> mem{MOS6502::MAX_MEM};
	MOS6502 cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, MOS6502::MAX_MEM);
	}

	virtual void TearDown() {
	}

	bool executeOneInstruction() {
		Cycles_t used;
		bool halt, debug;
		cpu.execute(halt, debug, used);
		return halt;
	}
};

TEST_F(MOS6502XXXFunctionalTestSuite, TestLoad6502TestSuite)
{
#ifdef RUN_FUNCTIONAL_TEST
	// Given:
	constexpr Word haltAddress = 0x3469;

	// When:
	mem.loadDataFromFile(fileName, 0x0000);
	cpu.setResetVector(0x0400);
	cpu.setHaltAddress(haltAddress);
	cpu.loopDetection(true); // Force a halt on 'jmp *'
	cpu.Reset();

	//Then:
	std::cout << "# This test takes about 20 seconds..." << std::endl;

	// Uncomment to start in debugger
	//cpu.SetDebug(true);
	
	while (!executeOneInstruction())
		;

	EXPECT_EQ(cpu.getPC(), haltAddress);
#endif
}

