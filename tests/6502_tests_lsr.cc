//
// Tests for lsr instruction
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

class MOS6502LSRTests : public testing::Test {
public:

	Memory<Address_t, Byte> mem{MOS6502::MAX_MEM};
	MOS6502 cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, MOS6502::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}
};

TEST_F(MOS6502LSRTests, LsrAccumulator) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_LSR_ACC;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	cpu.setA(0b01010101);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getA(), 0b00101010);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502LSRTests, LsrZeroPage) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_LSR_ZP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0x0020] = 0b01010101;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(mem[0x0020], 0b00101010);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502LSRTests, LsrZeroPageX) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_LSR_ZPX;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	cpu.setX(0x10);
	mem[0x0020] = 0b01010101;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(mem[0x0020], 0b00101010);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502LSRTests, LsrAbsolute) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_LSR_ABS;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0b01010101;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(mem[0x2000], 0b00101010);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502LSRTests, LsrAbsoluteX) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_LSR_ABX;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setX(0x05);
	mem[0x2005] = 0b01010101;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(mem[0x2005], 0b00101010);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

