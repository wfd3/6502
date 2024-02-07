//
// Tests for trb
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

class MOS65C02TRBTests : public testing::Test {
public:

	Memory<Word, Byte> mem{MOS65C02::LAST_ADDRESS};
	MOS65C02 cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, MOS65C02::LAST_ADDRESS);
	}
	
	virtual void TearDown()	{
	}
};

TEST_F(MOS65C02TRBTests, TRBAbsolute) {
    
	Byte ins = cpu.Opcodes.TRB_ABS;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
    Byte memval = 0x0f;
    Byte regval  = 0xf0;

	mem[0xfffc] = ins;
    mem[0xfffd] = 0x10;
    mem[0xfffe] = 0x10;
    mem[0x1010] = memval;
    cpu.setA(regval);

    EXPECT_EQ(mem[0x1010], memval);
    EXPECT_EQ(cpu.getA(), regval);
   
	//When:
	cpu.execute();

	// Then:
	EXPECT_EQ(mem[0x1010], memval & ~regval);

    bool zeroFlag = (memval & regval) == 0;
	EXPECT_EQ(cpu.getFlagZ(), zeroFlag);

	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles());  
}

TEST_F(MOS65C02TRBTests, TRBZeroPage) {
    
	Byte ins = cpu.Opcodes.TRB_ZP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
    Byte memval = 0x0f;
    Byte regval  = 0xf0;

	mem[0xfffc] = ins;
    mem[0xfffd] = 0x10;
    mem[0x10] = memval;
    cpu.setA(regval);

    EXPECT_EQ(mem[0x10], memval);
    EXPECT_EQ(cpu.getA(), regval);
   
	//When:
	cpu.execute();

	// Then:
	EXPECT_EQ(mem[0x10], memval & ~regval);

    bool zeroFlag = (memval & regval) == 0;
	EXPECT_EQ(cpu.getFlagZ(), zeroFlag);

	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles());  
}
