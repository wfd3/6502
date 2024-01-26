//
// Tests for bit instruction
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

class MOS65C02BITTests : public testing::Test {
public:
	Memory<Address_t, Byte> mem{MOS65C02::MAX_MEM};
	MOS65C02 cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, MOS65C02::MAX_MEM);
	}

	virtual void TearDown()	{
	}
};

#define testClass MOS65C02BITTests
#include "bit_tests.cc"

TEST_F(testClass, BitImmediate) {
	
	Byte ins = cpu.Opcodes.BIT_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x0f;
	cpu.setA(0xff);

	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, BitImmediateLeavesNFlagAlone) {
	
	Byte ins = cpu.Opcodes.BIT_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x0f;
	cpu.setA(0xff);
	cpu.setFlagN(true);

	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, BitImmediateLeavesVFlagAlone) {
	
	Byte ins = cpu.Opcodes.BIT_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x0f;
	cpu.setA(0xff);
	cpu.setFlagV(true);

	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, BitAbsoluteX) {
	
	Byte ins = cpu.Opcodes.BIT_ABX;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setX(0x10);
	mem[0x2010] = 0x0F;
	cpu.setA(0xff);

	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

TEST_F(testClass, BitZeroPageX) {
	
	Byte ins = cpu.Opcodes.BIT_ZPX;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x01;
	cpu.setX(0x2);
	mem[0x03] = 0x0f;
	cpu.setA(0xff);

	//When:

	//When:
	cpu.execute();

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagV());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

