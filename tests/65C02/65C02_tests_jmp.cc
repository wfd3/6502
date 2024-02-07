//
// Tests for jmp instructions for 65C02
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

class MOS65C02JMPTests : public testing::Test {
public:

	Memory<Word, Byte> mem{MOS65C02::MAX_MEM};
	MOS65C02 cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, MOS65C02::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}
};

#define testClass MOS65C02JMPTests
#include "jmp_tests.cc"

// Test that the 65C02 JMP instruction works as expected on 65C02
TEST_F(testClass, JmpIndirectBugIsFixedOn65C02) {
	
	Byte ins = cpu.Opcodes.JMP_IND;

	//Given:
	cpu.TestReset(MOS65C02::RESET_VECTOR);

	// JMP ($30ff)
	mem[0xfffc] = ins;
	mem[0xfffd] = 0xff;
	mem[0xfffe] = 0x30;

	mem[0x3000] = 0x40;
	mem[0x30ff] = 0x80;
	mem[0x3100] = 0x50;

	//When:
	cpu.execute();

	// Then:
	EXPECT_EQ(cpu.getPC(), 0x5080);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, JmpAbsoluteIndexedIndirect) {
	
	Byte ins = cpu.Opcodes.JMP_AII;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	mem[0xFFFE] = 0x10;
	cpu.setX(0x5);
	mem[0x1015] = 0x21;
	mem[0x1016] = 0x43;

	//When:
	cpu.execute();

	// Then:
	EXPECT_EQ(cpu.getPC(), 0x4321);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}
