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
// To disable this test comment out the next line
#define RUN_6502_FUNCTIONAL_TEST

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

#define testClass MOS6502XXXFunctionalTestSuite
#include "functional_tests.cc"