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

	Memory<Address_t, Byte> mem{CPU::MAX_MEM};
	CPU cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, CPU::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}
};

TEST_F(MOS6502JMPTests, JmpAbsolute) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_JMP_ABS;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x21;
	mem[0xFFFE] = 0x43;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getPC(), 0x4321);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502JMPTests, JmpIndirect) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_JMP_IND;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x21;
	mem[0x2001] = 0x43;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getPC(), 0x4321);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502JMPTests, JmpIndirectBug) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_JMP_IND;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);

	// JMP ($30ff)
	mem[0xfffc] = ins;
	mem[0xfffd] = 0xff;
	mem[0xfffe] = 0x30;

	mem[0x3000] = 0x40;
	mem[0x30ff] = 0x80;
	mem[0x3100] = 0x50;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getPC(), 0x4080);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
