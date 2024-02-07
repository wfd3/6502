//
// Tests for jmp instructions
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

class MOS6502JMPTests : public testing::Test {
public:

	Memory<Word, Byte> mem{MOS6502::LAST_ADDRESS};
	MOS6502 cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, MOS6502::LAST_ADDRESS);
	}
	
	virtual void TearDown()	{
	}
};

TEST_F(MOS6502JMPTests, JmpIndirectBug) {
	
	Byte ins = cpu.Opcodes.JMP_IND;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

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
	EXPECT_EQ(cpu.getPC(), 0x4080);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

#define testClass MOS6502JMPTests
#include "jmp_tests.cc"
