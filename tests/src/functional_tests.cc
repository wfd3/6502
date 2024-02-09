//
// Tests flags for 6502 only
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

#if !defined(testClass) 
# error "Macro 'testClass' not defined"
#endif

TEST_F(testClass, TestLoad6502TestSuite)
{
#ifdef RUN_6502_FUNCTIONAL_TEST
	// Given:
	static const char *fileName = BINFILE_PATH "/6502_functional_test.bin";
	constexpr Word haltAddress  = 0x3469;
	constexpr Word startAddress = 0x0400;

	// When:
	mem.loadDataFromFile(fileName, 0x0000);
	cpu.setResetVector(startAddress);
	cpu.setHaltAddress(haltAddress);
	cpu.enableLoopDetection(true); // Force a halt on 'jmp *'
	cpu.Reset();

	//Then:
	std::cout << "# 6502 Functional Test (can take 20 to 30 seconds)" << std::endl;

	// Uncomment to start in debugger
	//cpu.SetDebug(true);
	
	runProgram();

	EXPECT_EQ(cpu.getPC(), haltAddress);
#endif
}

TEST_F(testClass, TestLoad6502DecimalTestSuite)
{
#ifdef RUN_6502_DECIMAL_TEST
	// Given:
	static const char *fileName = BINFILE_PATH "/6502_decimal_test.bin";
	constexpr Word haltAddress  = 0x044b;
	constexpr Word startAddress = 0x0400;

	// When:
	mem.loadDataFromFile(fileName, 0x0000);
	cpu.setResetVector(startAddress);
	cpu.setHaltAddress(haltAddress);
	cpu.enableLoopDetection(true); // Force a halt on 'jmp *'
	cpu.Reset();

	//Then:
	std::cout << "# 6502 decimal tests" << std::endl;

	// Uncomment to start in debugger
	//cpu.SetDebug(true);
	
	runProgram();

	EXPECT_EQ(cpu.getPC(), haltAddress);
#endif
}

