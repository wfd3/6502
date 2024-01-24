//
// Tests for adc and sbc instructions
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

class MOS65C02ADCTests : public testing::Test {
public:
	Memory<Address_t, Byte> mem{MOS65C02::MAX_MEM};
	MOS65C02 cpu{mem};


	virtual void SetUp() {
		mem.mapRAM(0, MOS65C02::MAX_MEM);
	}

	virtual void TearDown()	{
	}
};

#define testClass MOS65C02ADCTests
#include "and_tests.cc"


TEST_F(testClass, AndZeroPageIndirect) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = cpu.Opcodes.AND_ZPI;
	
	// Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xfffd] = 0x50;
	mem[0x50] = 0x10;
	mem[0x51] = 0x10;
	mem[0x1010] = 0x0F;
	cpu.setA(0xff); 

	// When: 
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then
	EXPECT_EQ(cpu.getA(), 0x0f);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
