//
// Tests for stz
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

class MOS65C02STZTests : public testing::Test {
public:

	Memory<Address_t, Byte> mem{MOS65C02::MAX_MEM};
	MOS65C02 cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, MOS65C02::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}
};

TEST_F(MOS65C02STZTests, stz_absolute_zeros_memory) {
    
	Byte ins = cpu.Opcodes.STZ_ABS;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xfffc] = ins;
	mem[0xfffd] = 0x10;
    mem[0xfffe] = 0x10;

    mem[0x1010] = 0xff;

	//When:
	cpu.execute();

	// Then:
	EXPECT_EQ(mem[0x1010], 0);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(MOS65C02STZTests, stz_absolutex_zeros_memory) {
    
	Byte ins = cpu.Opcodes.STZ_ABX;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xfffc] = ins;
	mem[0xfffd] = 0x10;
    mem[0xfffe] = 0x10;
    cpu.setX(1);

    mem[0x1011] = 0xff;

	//When:
	cpu.execute();

	// Then:
	EXPECT_EQ(mem[0x1011], 0);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(MOS65C02STZTests, stz_zeropage_zeros_memory) {
    
	Byte ins = cpu.Opcodes.STZ_ZP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xfffc] = ins;
    mem[0xfffd] = 0x00;
	mem[0x00] = 0x10;

	//When:
	cpu.execute();

	// Then:
	EXPECT_EQ(mem[0x00], 0);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(MOS65C02STZTests, stz_zeropagex_zeros_memory) {
    
	Byte ins = cpu.Opcodes.STZ_ZPX;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xfffc] = ins;
    mem[0xfffd] = 0x00;
    cpu.setX(0x10);

	mem[0x10] = 0x10;

	//When:
	cpu.execute();

	// Then:
	EXPECT_EQ(mem[0x10], 0);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}