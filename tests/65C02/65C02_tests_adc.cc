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
	Memory<Word, Byte> mem{MOS65C02::LAST_ADDRESS};
	MOS65C02 cpu{mem};


	virtual void SetUp() {
		mem.mapRAM(0, MOS65C02::LAST_ADDRESS);
	}

	virtual void TearDown()	{
	}
};

#define testClass MOS65C02ADCTests
#include "adc_tests.cc"

TEST_F(testClass, ADCZeroPageIndirectAddsPositiveNumbers) {
	
	Byte ins = cpu.Opcodes.ADC_ZPI;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	mem[0x0010] = 0x10;
	mem[0x0011] = 0x10;
	mem[0x1010] = 0x10;
	cpu.setA(0x10);

	//When:
	cpu.execute();

	// Then:
	EXPECT_EQ(cpu.getA(), 0x20);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}
