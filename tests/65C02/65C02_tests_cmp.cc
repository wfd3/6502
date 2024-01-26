//
// Tests for cmp instruction 
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

class MOS65C02CMPTests : public testing::Test {
public:

	Memory<Address_t, Byte> mem{MOS65C02::MAX_MEM};
	MOS65C02 cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, MOS65C02::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}
};

#define testClass MOS65C02CMPTests
#include "brk_tests.cc"

TEST_F(testClass, CmpZeroPageIndirect) {
	
	Byte ins = cpu.Opcodes.CMP_ZPI;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0x0000] = 0x10;
	mem[0x0001] = 0x10;
	mem[0x1010] = 0x0f;
	cpu.setA(0x20);

	//When:
	cpu.execute();

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}