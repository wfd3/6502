//
// Tests for phx, plx, phy. ply
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

class MOS65C02pushpopTests : public testing::Test {
public:

	Memory<Word, Byte> mem{MOS65C02::MAX_MEM};
	MOS65C02 cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, MOS65C02::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}
};

TEST_F(MOS65C02pushpopTests, PHYImplied) {
    
	Byte ins = cpu.Opcodes.PHY_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setY(0x52);

	//When:
	cpu.execute();

	// Then:
	EXPECT_EQ(mem[0x01ff], 0x52);
	EXPECT_EQ(cpu.getSP(), MOS6502::INITIAL_SP - 1);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(MOS65C02pushpopTests, PLYImplied) {
	
	Byte ins = cpu.Opcodes.PLY_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR, MOS6502::INITIAL_SP - 1);
	
	mem[0xFFFC] = ins;
	mem[0x01FF] = 0x52;
	cpu.setY(0xff);

	//When:
	cpu.execute();

	// Then:
	EXPECT_EQ(cpu.getY(), 0x52);
	EXPECT_EQ(cpu.getSP(), MOS6502::INITIAL_SP);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(MOS65C02pushpopTests, PHXImplied) {
    
	Byte ins = cpu.Opcodes.PHX_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setX(0x52);

	//When:
	cpu.execute();

	// Then:
	EXPECT_EQ(mem[0x01ff], 0x52);
	EXPECT_EQ(cpu.getSP(), MOS6502::INITIAL_SP - 1);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(MOS65C02pushpopTests, PLXImplied) {
	
	Byte ins = cpu.Opcodes.PLX_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR, MOS6502::INITIAL_SP - 1);
	
	mem[0xFFFC] = ins;
	mem[0x01FF] = 0x52;
	cpu.setX(0xff);

	//When:
	cpu.execute();

	// Then:
	EXPECT_EQ(cpu.getX(), 0x52);
	EXPECT_EQ(cpu.getSP(), MOS6502::INITIAL_SP);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}