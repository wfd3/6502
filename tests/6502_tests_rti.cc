//
// Tests for rti instruction
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

class MOS6502RTITests : public testing::Test {
public:

	Memory<Address_t, Byte> mem{MOS6502::MAX_MEM};
	MOS6502 cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, MOS6502::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}
};

TEST_F(MOS6502RTITests, RtiImplied) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_RTI_IMP;

	//Given:
	cpu.TestReset(0x2000, MOS6502::INITIAL_SP - 3);
	mem[0x2000] = ins;
	mem[0x01FF] = 0xAA;
	mem[0x01FE] = 0xFF;
	mem[0x01FD] = 0x00;
	cpu.setPS(0xff);
	cpu.setFlagB(true);
	cpu.setFlagC(true);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getPC(), 0xAAFF);
	EXPECT_EQ(cpu.getSP(), MOS6502::INITIAL_SP);
	EXPECT_FALSE(cpu.getFlagB());
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
