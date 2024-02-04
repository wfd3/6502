//
// Runs the 65C02 functional tests from
//   https://github.com/Klaus2m5/65C02_65C02_functional_tests
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
#include <65C02.h>

// This test takes time to run.
// If the test completes, it passed.  If it drops into the debugger with a
// Loop Detected notice, it's failed.  
// 
// To disable this tests comment out the appropriate line
#define RUN_6502_FUNCTIONAL_TEST 
#define RUN_65C02_FUNCTIONAL_TEST

class MOS65C02FunctionalTestSuite : public testing::Test {
public:	
	Memory<Address_t, Byte> mem{MOS65C02::MAX_MEM};
	MOS65C02 cpu{mem};
	bool debug;

	virtual void SetUp() {
		mem.mapRAM(0, MOS65C02::MAX_MEM);
		std::cout << "## Functional tests will drop into debugger in case of failure" << std::endl;

		debug = false;
	}

	virtual void TearDown() {
	}

	void runProgram() {
		const char cursorChars[] = {'|', '/', '-', '\\'};
		const int numChars = sizeof(cursorChars) / sizeof(char);
		uint64_t instructionCount = 0;
		uint8_t cursorCount = 0;

		while (!cpu.isPCAtHaltAddress()) {
			cpu.execute();

			if (instructionCount % 1000000 == 0) {
				std::cout << cursorChars[cursorCount % numChars] << std::flush;
				std::cout << "\b" << std::flush;
				cursorCount++;
			}
			instructionCount++;
		}
		std::cout << " \b" << std::flush;
	}
};

#define testClass MOS65C02FunctionalTestSuite
#include "functional_tests.cc"

TEST_F(testClass, TestLoad65C02ExtendedOpcodesTestSuite)
{
#ifdef RUN_65C02_FUNCTIONAL_TEST
	// Given:
	static const char *fileName = BINFILE_PATH "/65C02_extended_opcodes_test_without_illegal_instructions.bin";
	constexpr Word haltAddress  = 0x1a95;
	constexpr Word startAddress = 0x0400;

	// When:
	mem.loadDataFromFile(fileName, 0x0000);
	cpu.setResetVector(startAddress);
	cpu.setHaltAddress(haltAddress);
	cpu.loopDetection(true); // Force a halt on 'jmp *'
	cpu.Reset();

	//Then:
	std::cout << "# 65C02 Extended Opcode Functional Tests (can take 20 to 30 seconds)" << std::endl;
	runProgram();
		
	EXPECT_EQ(cpu.getPC(), haltAddress);
#endif
}